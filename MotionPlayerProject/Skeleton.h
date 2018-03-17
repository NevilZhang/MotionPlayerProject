#ifndef _Skeleton_H_
#define _Skeleton_H_

#include "Bone.h"

//=================================================================
//定义骨架类。
class Skeleton
{
public:
	Skeleton() :bone_num(0) {};			 //构造函数	
	void set_weight();					 //给每一个骨节点设置权重值，离根节点越近越“重”
	void cal_local_coord();				 //计算全部骨节点在其父节点的“初始局部坐标系”下的局部坐标
	void cal_global_coord();			 //*计算全部骨节点的世界坐标

public:
	Bone bones[MAX_BONE_NUM];			 //核心数据结构，用以存储骨架骨骼数据，通过对象直接访问
	map<string, int> name_to_id;		 //还是定义为类的成员变量，用于绑定骨骼名字和id的
	int bone_num;						 //骨骼总数目
};
#endif











//==========================代码回收站=======================================
//void Test();										   //测试函数，检验相关结果是否正确。
//注：这里如果不将绘制函数设置为静态成员函数，glutDisplayFunc在后续将其注册成为回调函数时会出问题。因为其附带this指针
//但如果定义成为静态成员函数又会带来新的问题：静态成员函数无法访问非静态成员变量，因为前者不带this指针，找不到后者。
//我目前的权宜之计是：将绘制函数设置成为普通函数，而在main函数外部创建全局对象motion，这样普通函数就能访问数据成员了
//    void draw_skeleton();                                //骨架绘制函数，用以绘制初始状态骨骼。改为普通函数display了。
//    void Update();                                       //更新函数，用以实现键盘输入等交互式操作。同上。
//定义时间类。 
//class Timer
//{
//private:
//	int count, current;
//	double initial;
//	double *times;
//	double frequency;
//
//public:
//	Timer();
//	~Timer();
//	void initialize(int smooth);
//	void update(double Hz);
//	//void update(void);
//	//float avgTime(void);
//	//float avgHz(void);
//};



