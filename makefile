.PHONY:all
all:
	@cd compile_run_server;\
	make;\
	cd -;\
	cd oj_server;\
	make;\
	cd -;


.PHONY:output
output:
	@mkdir -p output/compile_run_server;\
	mkdir -p output/oj_server;\
	cp -rf compile_run_server/compile_run_server output/compile_run_server;\
	cp -rf compile_run_server/temp output/compile_run_server;\
	cp -rf oj_server/oj_server output/oj_server;\
	cp -rf oj_server/config output/oj_server;\
	cp -rf oj_server/questions output/oj_server;\
	cp -rf oj_server/template_html output/oj_server/;\
	cp -rf oj_server/wwwroot output/oj_server/;\


.PHONY:clean
clean:
	@cd compile_run_server;\
	make clean;\
	cd -;\
	cd oj_server;\
	make clean;\
	cd -;\
	rm -rf output;