all:
	g++ -o main *.cpp -pthread -ltbb
clean:
	rm main
