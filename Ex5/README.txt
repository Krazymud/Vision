code
	这个文件夹中存放代码
	可以在命令行输入g++ morph.cpp test.cpp -lgdi32进行编译
res1、res2、res3
	存放输出结果
	每个文件夹下有result.gif
a.exe
	运行后会要求输入图片地址、参数a、p、b，以及图片是否为灰度图，
	之后会进行描点，描点时命令行中会出现两边的点数（注意最后要一样多，cimg的鼠标事件检测有时会不准）
	描完点后会进行帧的生成，默认会存储在a.exe同目录下。