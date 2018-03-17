#ifndef _Motion_H_
#define _Motion_H_
#include "Skeleton.h"
//=================================================================
//�����˶��ࡣ
class Motion
{
public:
	Motion();											   //���캯�� 
	void Read_ASF(string filename);						   //��ȡ.ASF�ļ����ݡ�
	void Read_AMC(string filename);                        //��ȡ.AMC�ļ����ݣ��洢��N֡����	
	void cal_position();                                   //����ÿһ֡�Ǽ�������Ϣ

public:
	Skeleton skeletons[5000];                              //�ѹ�֮ǰ����5000֡�Ͳ�����~~o(>_<)o ~
	int total_frame_num;                                   //���˶�Ƭ����֡����

														   //friend�ɷ�������λ�ã�����private����publicӰ��
	friend void render_text();
	//friend void render_frame();
	friend void render();
	friend void reshape(int w, int h);
	friend void idle();
	friend void specialkey(int key, int x, int y);

};

#endif