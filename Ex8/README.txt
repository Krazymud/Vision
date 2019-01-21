包含第一部分代码与运行结果
A4文件夹下
	correct：保存A4纸矫正结果
	thres：保存自适应二值化结果
	seg：保存用于识别的字符分割结果
	a.exe：实现从边缘提取到字符分割的程序，使用方法：
		./a ImageData/1.jpg ImageData/2.jpg ...
		可以将输入的任意张图片分割，分割结果按图片序号-行号-行内位置.bmp保存在seg中

tensor文件夹下：
	运行cnn_predict.py可以预测单张图片，如：
		python cnn_predict.py x.jpg
	运行cnn_train.py可以进行模型训练，默认使用MNIST训练集
	运行cnn_run.py可以预测A4文件夹下seg中的若干图片，只要提供图片序号范围，如预测本次的十张图片：
		python cnn_run.py 0 10
	CNNResult：保存分类结果（图片）
	CNN-result.xlsx(csv)：以表格的形式存储预测结果
