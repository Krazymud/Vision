A4correction与Adaboost文件夹分别对应作业的两个部分
A4correction：
	参数.txt中的参数对应Dataset1中的六张测试图像，用来进行区域生长，
	使用txt中的参数可以获得比较好的结果。
	a.exe可直接运行
Adaboost：
	此为visual studio项目，需要先配置好visual studio的opencv才能编译运行
	main.cpp中有三个main函数，一个是读取图片进行手写数字识别的main函数，
	两个注释掉的main函数对应分类器的训练与测试（MNIST数据集）
	程序输入的图片默认在Adaboost文件夹下的testImg中，输入图片名即可读取
testImg中的图片。
	data文件夹下保存有MNIST数据集以及预测的结果和训练的model。