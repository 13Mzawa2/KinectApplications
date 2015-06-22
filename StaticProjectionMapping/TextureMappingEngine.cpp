#include "TextureMappingEngine.h"

using namespace cv;

TextureMappingEngine::TextureMappingEngine()
{
}


TextureMappingEngine::~TextureMappingEngine()
{
}

void TextureMappingEngine::staticTextureMapping(KinectV1 kinect)
{
	Mat temp;
	temp = imread("kinoko.jpg");
	hImg[0].loadTexture(temp);
	hImg[1].loadTexture(imread("takenoko.jpg"));
	hImg[2].loadTexture(imread("takenoko.jpg"));
	for (int idx = 0; idx < 3; idx++)
		hImg[idx].initProPoints();
	namedWindow("ControlWindow");
	setMouseCallback("ControlWindow", modifyVertexCallback, 0);

	Mat projectFrame = cv::Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);
	Mat controlFrame = cv::Mat(Size(PROJECTOR_WINDOW_WIDTH, PROJECTOR_WINDOW_HEIGHT), CV_8UC3);

	while (1)
	{
		//	kinectからの画像確認
		Mat colorImg;
		kinect.waitFrames();
		kinect.getColorFrame(colorImg);
		kinect.releaseFrames();
		flip(colorImg, colorImg, 1);		//	左右反転
		Mat colorImg_roi(colorImg, Rect(370, 140, 200, 250));
		Mat colorImg_rescale;
		resize(colorImg_roi, colorImg_rescale, Size(600, 750));
		imshow("確認用", colorImg_rescale);
		//	Frame背景を塗りつぶし
		projectFrame = Scalar(0,0,0);			//	黒（投影しない）
		controlFrame = Scalar(100, 100, 100);	//	暗めのグレー
		// Frameに対して四角形を描画
		for (int idx = 0; idx < 3; idx++)
		{
			hImg[idx].drawLayer(projectFrame);
			hImg[idx].drawLayer(controlFrame);
			hImg[idx].drawVertex(controlFrame);
		}
		imshow("ProjectorWindow", projectFrame);
		imshow("ControlWindow", controlFrame);


		//	キーイベント制御コード
		switch (waitKey(30))
		{
		case VK_ESCAPE:
			destroyWindow("ControlWindow");
			return ;
		case ' ':		//	フルスクリーン化
			fullscreen = !fullscreen;
			if (fullscreen)
			{
				setWindowProperty("ProjectorWindow", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
				cout << "フルスクリーンにしました" << endl;
			}
			else
			{
				setWindowProperty("ProjectorWindow", CV_WND_PROP_FULLSCREEN, CV_WINDOW_NORMAL);
			}
			break;
			//	テクスチャ選択
		case '1':
			selectedTextuerIndex = 0;
			cout << "テクスチャ0を選択" << endl;
			break;
		case '2':
			selectedTextuerIndex = 1;
			cout << "テクスチャ1を選択" << endl;
			break;
		case '3':
			selectedTextuerIndex = 2;
			cout << "テクスチャ2を選択" << endl;
			break;
			//	現在の座標系をセーブ
		case 's':
			saveTextureVertexData("Vertex.txt");
			break;
		case 'l':
			loadTextureVertexData("Vertex.txt");
			break;
		}

	}
}

void TextureMappingEngine::saveTextureVertexData(string filename)
{
	ofstream ofs(filename);
	for (int idx = 0; idx < 3; idx++)
	{
		for (int i = 0; i < 4; i++)
		{
			ofs << hImg[idx].proPoints[i] << endl;
		}
	}
}

void TextureMappingEngine::loadTextureVertexData(string filename)
{
	ifstream ifs(filename);

	if (ifs.fail()) {
		cerr << "File do not exist.\n";
		return;
	}
	for (int idx = 0; idx < 3; idx++)
	{
		for (int i = 0; i < 4; i++)
		{
			string str;
			if (!getline(ifs, str)) break;
			Point2f p;
			sscanf_s(str.data(), "[%f,%f]", &p.x, &p.y);
			hImg[idx].proPoints[i] = p;
			
		}
	}
	
}