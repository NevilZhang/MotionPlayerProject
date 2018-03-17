#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <GL/glut.h>
using namespace std;

#define PI						3.14159265358979323846
#define EPS						0.00000000000000000001
#define MAX_FRAME_NUM			500
#define MAX_BONE_NUM			31
#define eps						0.0000000000001


//===============================================================================
//定义向量与运算函数
class VECTOR {
public:
	VECTOR() :x(0.0), y(0.0), z(0.0) {}
public:
	double x, y, z;
};
//1、向量归一化
VECTOR normalize_vec(VECTOR &vec);

//2、计算两个向量的夹角
double cal_angle(VECTOR &v1, VECTOR &v2);

//3、计算两个向量的法向量,并使结果归一化。
VECTOR cal_normal_vec(VECTOR &v1, VECTOR &v2);


//================================================
//定义矩阵类型数据以及相关矩阵运算
typedef struct
{
	double Index[3][3];
}Matrix;

//1、矩阵加法
Matrix MatrixPlus(Matrix &, Matrix &);

//2、矩阵减法
Matrix MatrixSub(Matrix &, Matrix &);

//3、矩阵乘法
Matrix MatrixMult(Matrix &, Matrix &);

//4、矩阵乘向量
VECTOR MatrixMultVec(Matrix &, VECTOR &);

//5、矩阵求逆
Matrix MatrixInverse(Matrix &);

//6、将向量绕X轴旋转angle角度
VECTOR Matrix_RotateX(double, VECTOR &);

//7、将向量绕Y轴旋转angle角度
VECTOR Matrix_RotateY(double, VECTOR &);

//8、将向量绕Z轴旋转angle角度
VECTOR Matrix_RotateZ(double, VECTOR &);


#endif