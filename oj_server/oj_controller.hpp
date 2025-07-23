#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <jsoncpp/json/json.h>
#include <cassert>
#include <fstream>
#include <memory>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "../comm/httplib.h"

// #include "oj_model_file.hpp"
#include "oj_model_mysql.hpp"

#include "oj_view.hpp"

namespace ns_controller {
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;

    const std::string service_machine_config_path = "./config/service_machines.conf";

    struct Machine {
        std::string _ip;
        int _port;
        uint32_t _load;
        std::shared_ptr<std::mutex> _p_mtx; // mutex 本身禁止拷贝，使用智能指针
    public:
        Machine(const std::string& ip = "", int port = 0, uint32_t load = 0, std::shared_ptr<std::mutex> p_mtx = nullptr)
            : _ip(ip), _port(port), _load(load), _p_mtx(p_mtx) {}
        void InseaseLoad() {
            if(_p_mtx) _p_mtx->lock();
            _load++;    
            if(_p_mtx) _p_mtx->unlock();
        }
        void DecreaseLoad() {
            if(_p_mtx) _p_mtx->lock();
            _load--;    
            if(_p_mtx) _p_mtx->unlock();
        }
        uint32_t GetLoad() const {
            if(_p_mtx) _p_mtx->lock();
            uint32_t load = _load;
            if(_p_mtx) _p_mtx->unlock();
            return load;
        }
    };

    class LoadBalancer {
        std::vector<Machine> _machines;  // 所有主机
        std::vector<int> _online;        // 所有在线的主机id 使用每台主机的下标作为其id
        std::vector<int> _offline;       // 所有下线的主机id
        std::mutex _mtx;                 // 主机列表的互斥锁
    private:
        bool LoadConfig(const std::string& config_path) {
            std::ifstream ifs(config_path);
            if(!ifs.is_open()) {
                LOG(FATAL) << "加载：" << config_path << "失败" << std::endl;
                return false;
            }
            std::string line;
            while(std::getline(ifs, line)) {
                std::vector<std::string> tokens;
                StringUtil::SpiltString(line, tokens, ":");
                if(tokens.size() != 2) {
                    LOG(WARNING) << "配置文件中某个 Machnie 格式存在错误：" << line << std::endl;
                    continue;
                }
                Machine m(tokens[0], std::stoi(tokens[1]), 0, std::make_shared<std::mutex>()); 
                _machines.push_back(m);
                _online.push_back(_machines.size() - 1);
            }
            ifs.close();
            return true;
        }

        void ResetLoad(int id) {
            _machines[id]._p_mtx->lock();
            _machines[id]._load = 0; 
            _machines[id]._p_mtx->unlock();
        }

    public:
        LoadBalancer() {
            assert(LoadConfig(service_machine_config_path));
            LOG(INFO) << "加载主机配置: " << service_machine_config_path <<" 成功" << std::endl;
        }

        // int& id         输出型参数
        // Machine** pp_m  输出型参数，获取_machines中对应id的Machine去使用
        bool SmartChooseMachine(int& id, Machine** pp_m) {
            _mtx.lock();

            // 负载均衡算法:
            // 1. 随机数+hash
            // 2. 轮询 + hash
            // 此处选择使用轮询 + hash
            int online_num = _online.size();
            if(online_num == 0) {
                _mtx.unlock();
                return false;
            }
            uint32_t min_load = _machines[_online[0]].GetLoad();
            id = _online[0];
            *pp_m = &_machines[id];
            for(int i = 1; i < online_num; i++) {
                uint32_t temp_load = _machines[_online[i]].GetLoad();
                if(temp_load < min_load) {
                    min_load = temp_load;
                    id = _online[i];
                    *pp_m = &_machines[id];
                }
            }
            _mtx.unlock();
            return true;
        }

        void OfflineMachine(int id) {
            _mtx.lock();
            _offline.push_back(id);
            _online.erase(std::remove(_online.begin(), _online.end(), id), _online.end());
            // 下线负载置零
            ResetLoad(id);
            _mtx.unlock();
        }
        void OnlineMachine(int id) {
            _mtx.lock();
            _online.push_back(id);
            _offline.erase(std::remove(_offline.begin(), _offline.end(), id), _offline.end());
            _mtx.unlock();
        }

        // 统一上线所有主机
        void OnlineAllMachines() {
            _mtx.lock();
            _online.insert(_online.end(), _offline.begin(), _offline.end());
            _offline.clear();
            _mtx.unlock();
        }

        // for debug
        void ShowMachines() {
            _mtx.lock();
            std::cout << "当前在线主机：" << std::endl;
            for(int i = 0; i < _online.size(); i++) {
                std::cout << "主机：" << _online[i] << " " << _machines[_online[i]]._ip << ":" << _machines[_online[i]]._port << " 负载：" << _machines[_online[i]]._load << std::endl;
            }
            std::cout << "当前下线主机：" << std::endl;
            for(int i = 0; i < _offline.size(); i++) {
                std::cout << "主机：" << _offline[i] << " " << _machines[_offline[i]]._ip << ":" << _machines[_offline[i]]._port << " 负载：" << _machines[_offline[i]]._load << std::endl;
            }
            _mtx.unlock();
        }
    };


    class Controller {
        Model _model;
        View _view;
        LoadBalancer _load_balancer; // 负责负载均衡
    public:
        void Recovery() {
            _load_balancer.OnlineAllMachines();
            LOG(INFO) << "所有主机已上线" << std::endl;
        }

        // std::string& html_str 输出型参数
        bool GetAllQuestionsHtml(std::string& html_str) {
            std::vector<Question> qs;
            if(_model.GetAllQuestions(qs)) {
                sort(qs.begin(), qs.end(), [](const Question& a, const Question& b) {
                    return atoi(a.id.c_str()) < atoi(b.id.c_str());
                });
                // 获取所有题目完成，构建网页输出
                _view.ExpandTemplate(qs, html_str);
                return true;
            }
            else {
                html_str = "获取所有题目失败";
                return false;
            }
        }

        bool GetQuestionHtml(const std::string& ques_id, std::string& html_str) {
            Question q;
            if(_model.GetOneQuestion(ques_id, q)) {
                // 获取指定题目完成，构建网页输出
                _view.ExpandTemplate(q, html_str);
                return true;
            }
            else {
                html_str = "获取题目[" + q.id + "]失败";
                return false;
            }
        }

        // in_json_str：
        //    id: 100
        //    code: #include <iostream> ...
        //    input: ""
        void Judge(const std::string& ques_id, const std::string& in_json_str, std::string& out_json_str) {
            // LOG(DEBUG) << in_json_str << std::endl << "quess_id: " << ques_id << std::endl;
            // 0.根据题目直接拿到对应的题目细节
            Question q;
            _model.GetOneQuestion(ques_id, q);
            // 1.对 in_ json_str 进行反序列化，得到题目数据
            Json::Value in_json;
            Json::Reader reader;
            reader.parse(in_json_str, in_json);
            // 2.重新拼接用户代码+测试代码
            Json::Value compile_json;
            compile_json["code"] = in_json["code"].asString() + "\n" + q.tail;
            compile_json["input"] = in_json["input"].asString();
            compile_json["cpu_limit"] = q.cpu_limit;
            compile_json["mem_limit"] = q.mem_limit;
            Json::FastWriter writer;
            std::string compile_json_str = writer.write(compile_json);
            // 3.选择负载最低的编译运行服务，发起 http 请求得到结果
            while(true) { // 一直选择，直到选到一个可用编译服务
                int machine_id;
                Machine* p_machine;
                if(!_load_balancer.SmartChooseMachine(machine_id, &p_machine)) {
                    LOG(FATAL) << "所有后端编译服务主机已下线，需要运维同时尽快查看" << std::endl;
                    break;
                }
                LOG(INFO) << "选择主机：" << machine_id << " " << p_machine->_ip << ":" << p_machine->_port << " 进行编译运行 " << "此主机当前负载：" << p_machine->_load << std::endl;
                // 4.发起 http 请求
                p_machine->InseaseLoad(); // 更新主机负载
                httplib::Client cli(p_machine->_ip.c_str(), p_machine->_port);
                if(auto res = cli.Post("/compile_run", compile_json_str, "application/json; charset=utf-8")) { // 请求成功
                    if(res->status == 200) { // 编译运行成功，将结果赋值给 out_json_str
                        LOG(INFO) << "主机：" << machine_id << " " << p_machine->_ip << ":" << p_machine->_port << " 已成功处理请求并返回响应" << std::endl;
                        out_json_str = res->body;
                        p_machine->DecreaseLoad(); // 更新主机负载
                        break;
                    }
                    else { 
                        LOG(ERROR) << "主机：" << machine_id << " " << p_machine->_ip << ":" << p_machine->_port << " 处理请求失败 状态码：" << res->status << std::endl;
                        p_machine->DecreaseLoad(); // 更新主机负载
                        continue;
                    }
                }
                else { // 如果请求失败，则下线该主机，重新选择
                    LOG(ERROR) << "请求主机：" << machine_id << " " << p_machine->_ip << ":" << p_machine->_port << " 失败，可能已经下线" << std::endl;
                    p_machine->DecreaseLoad(); // 可以不写，因为下线后会自动清零
                    _load_balancer.OfflineMachine(machine_id);
                    _load_balancer.ShowMachines(); // For debug
                    continue;
                }
            }
        }
    };
}

