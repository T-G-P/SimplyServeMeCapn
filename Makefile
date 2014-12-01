all: SNFSclient SNFSserver

testing: SNFSclient SNFSserver throughput

SNFSserver:
	gcc -g -pthread serverSNFS.c -o SNFSserver

SNFSclient:
	gcc -g -lrt clientapp.c clientSNFS.c -o SNFSclient

Throughput:
	gcc -g -pthread -lrt throughput.c clientSNFS.c -o throughput

clean:
	rm -f SNFSclient SNFSserver portnum throughput

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

testthrough: clean all
	killall SNFSserver || true
	killall throughput || true
	bash -c 'echo $$RANDOM > portnum'
	@echo -n "port: "
	@cat portnum
	@echo ------- starting client -------
	@rm through.log || true
	bash -c 'sleep 1 && ./throughput -host localhost -port `cat portnum` > through.log' &
	@echo ------- starting server -------
	bash -c './SNFSserver -port `cat portnum` -mount `mktemp -d`'
	@echo 'sleeping...'
	@bash -c 'sleep 1'
