������һ���ִ��������н��
A4�ļ�����
	correct������A4ֽ�������
	thres����������Ӧ��ֵ�����
	seg����������ʶ����ַ��ָ���
	a.exe��ʵ�ִӱ�Ե��ȡ���ַ��ָ�ĳ���ʹ�÷�����
		./a ImageData/1.jpg ImageData/2.jpg ...
		���Խ������������ͼƬ�ָ�ָ�����ͼƬ���-�к�-����λ��.bmp������seg��

tensor�ļ����£�
	����cnn_predict.py����Ԥ�ⵥ��ͼƬ���磺
		python cnn_predict.py x.jpg
	����cnn_train.py���Խ���ģ��ѵ����Ĭ��ʹ��MNISTѵ����
	����cnn_run.py����Ԥ��A4�ļ�����seg�е�����ͼƬ��ֻҪ�ṩͼƬ��ŷ�Χ����Ԥ�Ȿ�ε�ʮ��ͼƬ��
		python cnn_run.py 0 10
	CNNResult�������������ͼƬ��
	CNN-result.xlsx(csv)���Ա�����ʽ�洢Ԥ����
