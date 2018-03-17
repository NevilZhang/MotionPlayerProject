#ifndef _Motion_H_
#define _Motion_H_
#include "Skeleton.h"
//=================================================================
//定义运动类。
class Motion
{
public:
	Motion();											   //构造函数 
	void Read_ASF(string filename);						   //读取.ASF文件内容。
	void Read_AMC(string filename);                        //读取.AMC文件内容，存储第N帧数据	
	void cal_position();                                   //计算每一帧骨架坐标信息

public:
	Skeleton skeletons[5000];                              //难怪之前超过5000帧就不行了~~o(>_<)o ~
	int total_frame_num;                                   //该运动片段总帧数。

														   //friend可放在任意位置，不受private或者public影响
	friend void render_text();
	//friend void render_frame();
	friend void render();
	friend void reshape(int w, int h);
	friend void idle();
	friend void specialkey(int key, int x, int y);

};

#endif