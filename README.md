# 概要

アプリケーションの性能を測定したい時に、あったらいいなという思う機能をつめこんだパフォーマンスログ出力クラスとそのビューアーアプリをつくりました。

![viewer](/image/PerformanceViewer.png)

ログを仕込んだポイントの情報をテキストファイル（CSV形式）で出力します。ビューアーアプリで見たときに、どの関数がどのタイミングで実行されて、どれくらい時間がかかったかを可視化して見ることができます。よくあるデバッガのタイムライン機能みたいなものです。使いこなせればそこそこ便利だと思われます。C++ 専用です。バグはあるかもしれません。自己責任でお使いください。ちなみに Windows でしか使えませんが簡単に改造できると思います。 

# 使用方法

## 1. 準備

まずは [PerfomanceLogger クラス](https://github.com/ii0244is/PerformanceLogger/tree/master/Logger)
のソースコードをゲットして、ターゲットアプリのプロジェクトに加えます。ソースコード上のログを出力したいところにコードを埋め込んでいく形になります。

## 2. ログを仕込む

ロガークラスは基本的にシングルトンでインスタンスが一つしか生成されません。ですのでどのソースファイルからでもヘッダをインクルードするだけで使えます。PerformanceLogger::getInstance() みたいな形でロガークラスのインスタンスを取得して使います。 

はじめに startLogger() を呼び出してロガーをスタートさせる必要があります。この関数の引数にはログファイルの名前を自由に決めて入力してやります。 これを最初にやっておかないとログファイルは生成されず何も出力されません。 逆にいえばログの取得を開始したいタイミングで呼び出してやることにより、余計なログが出力されなくて済みます。

```
PerformanceLogger::getInstance()->startLogger("logTest.csv");
```

パフォーマンスを測定したい場所を { } で囲んでスコープを限定し、 ProcessingTimeLogger クラスのインスタンスを生成します。コンストラクタの引数で自由にコメントを残せます。

```
{
    ProcessingTimeLogger logger("UserComment");
    パフォーマンスを計測したい処理;
}
```
雰囲気はこんな感じです。
```
#include "../Logger/PerformanceLogger.h"

int main()
{
    PerformanceLogger::getInstance()->startLogger("logTest.csv");

    ProcessingTimeLogger logMain("main");

    Sleep(300);

    {
        ProcessingTimeLogger log("Test1");
        Sleep(200);
    }

    Sleep(150);

    {
        ProcessingTimeLogger log("Test2");
        Sleep(500);
    }

    Sleep(600);

    return 0;
}
```

## 3. 出力ファイル


```
Time[us],ThreadID,UserComment1,UserComment2,val1,val2
4131.000000,21652,main,start,4120.000000,0.000000
304307.000000,21652,Test1,start,304306.000000,0.000000
510054.000000,21652,Test1,stop,304306.000000,205746.000000
673116.000000,21652,Test2,start,673116.000000,0.000000
1174240.000000,21652,Test2,stop,673116.000000,501122.000000
1782168.000000,21652,main,stop,4120.000000,1778046.000000
```

## 4. ビューアー
出力されたログ情報はビューアーアプリで可視化して見ることができます。ビューアーアプリは[こちら](https://github.com/ii0244is/PerformanceLogger/tree/master/Viewer)です。 テストは全然やってないので動かなかったらごめんなさい。 

Web アプリなのでこちらからも実行できます。  
https://ii0244is.github.io/PerformanceViewer/

結果はこんな感じ。
![sample1](/image/sample1.png)

# サンプル
この機能の真骨頂はマルチスレッドなプログラムで発揮されます。例えば下記のようなスレッド乱立、排他制御しまくりのプログラムでも、各処理の順序を可視化して俯瞰することにより、どいつがパフォーマンスのボトルネックになっているかを探し出すことができ、性能改善のための手がかりを得ることができるかもしれません。

```
#include "../Logger/PerformanceLogger.h"

std::mutex mutexSection1;
std::mutex mutexSection2;

void criticalSection1()
{
	ProcessingTimeLogger log("CriticalSection1");

	for (int i = 0; i < 300000; ++i)
	{
		double a, b, c;
		a = (double)(rand() * i);
		b = (double)(rand() * i);
		c = a * a + b * b;
	}
}

void criticalSection2()
{
	ProcessingTimeLogger log("CriticalSection2");

	for (int i = 0; i < 450000; ++i)
	{
		double a, b, c;
		a = (double)(rand() * i);
		b = (double)(rand() * i);
		c = a * a + b * b;
	}
}

void workerThread1()
{
	ProcessingTimeLogger workerlog("WorkerThread1");

	{
		ProcessingTimeLogger log("CriticalSection1 Wait");
		mutexSection1.lock();
	}
	criticalSection1();
	mutexSection1.unlock();

	{
		ProcessingTimeLogger log("CriticalSection2 Wait");
		mutexSection2.lock();
	}
	criticalSection2();
	mutexSection2.unlock();
}

void workerThread2()
{
	ProcessingTimeLogger workerlog("WorkerThread2");

	{
		ProcessingTimeLogger log("CriticalSection2 Wait");
		mutexSection2.lock();
	}
	criticalSection2();
	mutexSection2.unlock();

	{
		ProcessingTimeLogger log("CriticalSection1 Wait");
		mutexSection1.lock();
	}
	criticalSection1();
	mutexSection1.unlock();
}

int main()
{
	PerformanceLogger::getInstance()->startLogger("logTest.csv");

	ProcessingTimeLogger log("main");

	Sleep(10);
	std::thread threadA(workerThread1);
	std::thread threadB(workerThread2);
	std::thread threadC(workerThread1);
	std::thread threadD(workerThread2);
	Sleep(10);
	std::thread threadE(workerThread1);
	std::thread threadF(workerThread1);
	std::thread threadG(workerThread2);

	threadA.join();
	threadB.join();
	threadC.join();
	threadD.join();
	threadE.join();
	threadF.join();
	threadG.join();

	return 0;
}
```

このサンプルの実行結果は[こちら](https://github.com/ii0244is/PerformanceLogger/tree/master/Sample/Sample2)です。ミューテックスロックの前後にログを仕込むことにより、クリティカルセクションに入る前にどれだけが待ち時間であったかも可視化されてわかるようになります。 
