#!/usr/bin/python3
import matplotlib.pyplot as plt
import subprocess
from datetime import datetime

prog_path = './cmake-build-release/untitled'

def run_prog(n: int) -> int:
	start = datetime.now().timestamp()
	res = subprocess.run([prog_path], text=True, input=str(n))
	print(res)
	return datetime.now().timestamp() - start

def main():
	N = range(1, 50)
	times = [run_prog(x) for x in N]
	plt.plot(N, times) 
	plt.xlabel('N - размер квадрата') 
	plt.ylabel('Время выполнения программы') 
	plt.title('Зависимость времени выполнения от размера квадрата') 
	plt.show() 


if __name__ == "__main__":
	main()