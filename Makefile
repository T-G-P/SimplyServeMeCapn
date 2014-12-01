all: SNFSclient SNFSserver

SNFSserver:
	gcc -g -pthread serverSNFS.c -o SNFSserver

SNFSclient:
	gcc -g -lrt clientapp.c clientSNFS.c -o SNFSclient

clean:
	rm -f SNFSclient SNFSserver portnum

test: clean all
	killall SNFSserver || true
	killall SNFSclient || true
	bash -c 'echo $$RANDOM > portnum'
	@echo -n "port: "
	@cat portnum
	@echo ------- starting server -------
	@rm server.log || true
	bash -c './SNFSserver -port `cat portnum` -mount `mktemp -d` &> server.log &'
	@echo 'sleeping...'
	@bash -c 'sleep 1'
	@echo ------- starting client -------
	bash -c './SNFSclient -host localhost -port `cat portnum`'


testserver: clean all
	killall SNFSserver || true
	killall SNFSclient || true
	bash -c 'echo $$RANDOM > portnum'
	@echo -n "port: "
	@cat portnum
	@echo ------- starting client -------
	@rm client.log || true
	bash -c 'sleep 1 && ./SNFSclient -host localhost -port `cat portnum` > client.log' &
	@echo ------- starting server -------
	bash -c './SNFSserver -port `cat portnum` -mount `mktemp -d`'
	@echo 'sleeping...'
	@bash -c 'sleep 1'

