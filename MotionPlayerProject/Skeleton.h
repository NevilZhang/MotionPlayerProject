#ifndef _Skeleton_H_
#define _Skeleton_H_

#include "Bone.h"

//=================================================================
//����Ǽ��ࡣ
class Skeleton
{
public:
	Skeleton() :bone_num(0) {};			 //���캯��	
	void set_weight();					 //��ÿһ���ǽڵ�����Ȩ��ֵ������ڵ�Խ��Խ���ء�
	void cal_local_coord();				 //����ȫ���ǽڵ����丸�ڵ�ġ���ʼ�ֲ�����ϵ���µľֲ�����
	void cal_global_coord();			 //*����ȫ���ǽڵ����������

public:
	Bone bones[MAX_BONE_NUM];			 //�������ݽṹ�����Դ洢�Ǽܹ������ݣ�ͨ������ֱ�ӷ���
	map<string, int> name_to_id;		 //���Ƕ���Ϊ��ĳ�Ա���������ڰ󶨹������ֺ�id��
	int bone_num;						 //��������Ŀ
};
#endif











//==========================�������վ=======================================
//void Test();										   //���Ժ�����������ؽ���Ƿ���ȷ��
//ע����������������ƺ�������Ϊ��̬��Ա������glutDisplayFunc�ں�������ע���Ϊ�ص�����ʱ������⡣��Ϊ�丽��thisָ��
//����������Ϊ��̬��Ա�����ֻ�����µ����⣺��̬��Ա�����޷����ʷǾ�̬��Ա��������Ϊǰ�߲���thisָ�룬�Ҳ������ߡ�
//��Ŀǰ��Ȩ��֮���ǣ������ƺ������ó�Ϊ��ͨ����������main�����ⲿ����ȫ�ֶ���motion��������ͨ�������ܷ������ݳ�Ա��
//    void draw_skeleton();                                //�Ǽܻ��ƺ��������Ի��Ƴ�ʼ״̬��������Ϊ��ͨ����display�ˡ�
//    void Update();                                       //���º���������ʵ�ּ�������Ƚ���ʽ������ͬ�ϡ�
//����ʱ���ࡣ 
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



