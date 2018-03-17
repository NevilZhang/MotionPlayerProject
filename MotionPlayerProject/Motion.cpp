#include "Motion.h"
//==========================以下是Motion类成员函数的定义==================================//
//定义默认构造函数。
Motion::Motion()
{
	total_frame_num = 0; //运动片段总帧数
}

//定义成员函数：读取ASF文件。
void Motion::Read_ASF(string filename)
{

	//打开输入流，读取文件
	ifstream data(filename);
	if (!data) cerr << "asf文件打开失败!" << endl;

	string line = "0";
	string key_words = "0";		    //用于保存关键字
	string	tag = "0";			    //tag用以标明该行所属状态
									//首先处理根节点，初始化	
	skeletons[0].bones[0].id = 0;
	skeletons[0].bones[0].name = "root";
	skeletons[0].bones[0].length = 0.0;
	skeletons[0].name_to_id["root"] = 0;
	//以上，根节点处理完毕

	//大循环，控制整个文件的逐行读入
	while (getline(data, line))
	{
		stringstream ss(line);
		ss >> key_words;
		//以下设置状态标记
		if (key_words == ":root")      tag = ":root";
		if (key_words == ":bonedata")  tag = ":bonedata";
		if (key_words == ":hierarchy") tag = ":hierarchy";
		//至此，状态标记设置完毕	
		//问题就出在这里，只能用索引更改一个已经赋值的元素，但不能使用索引来初始化编号为i的元素。
		//当需要向向量中首次添加一个元素时，通常要使用成员函数push_back();所以改用对象数组了。

		if (key_words == "position" && tag == ":root")      //目前，读到了根节点坐标
		{
			ss >> skeletons[0].bones[0].global_coord.x;
			ss >> skeletons[0].bones[0].global_coord.y;
			ss >> skeletons[0].bones[0].global_coord.z;
		}

		if (key_words == "orientation" && tag == ":root")   //目前，读到了根节点初始局部坐标系
		{
			ss >> skeletons[0].bones[0].local_coord_axis.x;
			ss >> skeletons[0].bones[0].local_coord_axis.y;
			ss >> skeletons[0].bones[0].local_coord_axis.z;
			//ASF数据文件中的数据是角度，这里应该转为弧度，否则后续计算会出错
			skeletons[0].bones[0].local_coord_axis.x = skeletons[0].bones[0].local_coord_axis.x / 180.0*PI;
			skeletons[0].bones[0].local_coord_axis.y = skeletons[0].bones[0].local_coord_axis.y / 180.0*PI;
			skeletons[0].bones[0].local_coord_axis.z = skeletons[0].bones[0].local_coord_axis.z / 180.0*PI;

		}
		//以上，根节点处理完毕。
		//注：其余关键字暂时自动忽略。

		//以下处于:bonedata状态下，接下来还需要将不同的骨骼区别开来
		if (key_words == "begin" && tag == ":bonedata")      //目前，正式开始骨骼数据
		{
			skeletons[0].bone_num++;
		}

		if (key_words == "id" && tag == ":bonedata")         //目前，读到了骨骼id
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].id;
			//进行一个检测，当前骨骼数应当等于当前骨骼id，否则报错。
			if (skeletons[0].bone_num != skeletons[0].bones[skeletons[0].bone_num].id)
				cerr << "There must be Errors!" << endl;
		}

		if (key_words == "name" && tag == ":bonedata")       //目前，读到了骨骼name
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].name;
			skeletons[0].name_to_id[skeletons[0].bones[skeletons[0].bone_num].name] =
				skeletons[0].bones[skeletons[0].bone_num].id; //在这里，通过map，绑定骨骼名字和id
		}

		if (key_words == "direction" && tag == ":bonedata")  //目前，读到了骨骼初始direction
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.x;
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.y;
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.z;
		}

		if (key_words == "length" && tag == ":bonedata")     //目前，读到了骨骼length
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].length;
		}

		if (key_words == "axis" && tag == ":bonedata")       //目前，读到了骨骼axis，即局部坐标轴
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x;
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y;
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z;
			//ASF数据文件中的数据是角度，这里应该转为弧度，否则后续计算会出错。
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x / 180.0*PI;
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y / 180.0*PI;
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z / 180.0*PI;
		}

		if (key_words == "dof" && tag == ":bonedata")        //目前，读到了骨骼dof，即自由度
		{
			string temp = "0";
			while (ss >> temp)
			{
				if (temp == "rx")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[0] = 1;     //相当于打开开关
				if (temp == "ry")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[1] = 1;		//相当于打开开关
				if (temp == "rz")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[2] = 1;		//相当于打开开关
			}
		}
		//注：这里直接无视掉limits了...囧rz...

		//以下处于:hierarchy状态下：
		//样例数据：root lhipjoint rhipjoint lowerback
		//          thorax lowerneck lclavicle rclavicle

		if (key_words != ":hierarchy" &&key_words != "begin"&& tag == ":hierarchy")
		{
			if (key_words == "end") break;				//此时意味着读到了整个数据的结尾了
			string temp;
			while (ss >> temp)                         //该行还有，就一直读
			{
				//两件事：1、将关键字节点添加到后续节点的父节点 2、将后续节点加为关键字节点的子节点
				skeletons[0].bones[skeletons[0].name_to_id[key_words]].child_id.push_back(skeletons[0].name_to_id[temp]);
				skeletons[0].bones[skeletons[0].name_to_id[temp]].father_id = skeletons[0].name_to_id[key_words];
			}
		}											//End of if

	}												//End of while(getline(data,line))

													//最后关闭流
	data.close();

}													//End of Skeleton::Read_ASF(char *filename)


													//定义AMC文件读取函数，用以存储全部帧数据。
void Motion::Read_AMC(string filename)
{

	//首先打开输入流
	ifstream data(filename);
	if (!data) cerr << "amc文件读取失败!" << endl;

	int curr_frame_num = 0;   //用以保存所读取的行所处的帧数，要和main里面的加以区分。
	total_frame_num = 0;      //每次读取归零可解决帧数多的运动切换到帧数少的运动后继续播放先前运动的问题。
	string line = "0";
	while (getline(data, line))      //大循环，控制整个文件的逐行读入
	{
		//读取一行的第一个单词
		string key_words = "0";
		stringstream ss(line);
		ss >> key_words;

		//理一理逻辑，应该是这样的：对于任意一行，均同等对待，但对关键字需要作出一个判断:
		//如果是数字，当前帧状态设置为该数字，直到遇到下一个数字状态才会发生改变

		//对于相应帧，利用静态数据成员map，将后续值存入相应骨骼的dof[3]，当然需要结合前序的dofflag[]辅助判断。
		if (isdigit(key_words[0]))					 //表示当前“读入行”是数字
		{
			//首先还要将字符串转化为数字
			istringstream temp_string;               //临时字符串辅助转化
			temp_string.str(key_words);
			temp_string >> curr_frame_num;             //在这里更新当前帧数

			skeletons[curr_frame_num] = skeletons[0];  //*把skeletons[0]的骨架数据复制给当前帧的骨架数据。
													   //因为所有帧共用一个骨架。
													   //更新最大帧数目。
			if (curr_frame_num > total_frame_num)
				total_frame_num = curr_frame_num;

		}
		else
		{
			//加载自由度数据，即各骨骼绕父节点局部坐标系旋转的角度
			//需要把根节点单独拎出来处理，这里根节点的全局坐标比较特殊，直接有了，无需计算。
			if (key_words == "root")
			{
				ss >> skeletons[curr_frame_num].bones[0].global_coord.x;
				ss >> skeletons[curr_frame_num].bones[0].global_coord.y;
				ss >> skeletons[curr_frame_num].bones[0].global_coord.z;

				ss >> skeletons[curr_frame_num].bones[0].dof[0];
				ss >> skeletons[curr_frame_num].bones[0].dof[1];
				ss >> skeletons[curr_frame_num].bones[0].dof[2];
				//AMC数据文件中的数据是角度，这里应该转为弧度，否则后续计算会出错。
				skeletons[curr_frame_num].bones[0].dof[0] = skeletons[curr_frame_num].bones[0].dof[0] / 180.0*PI;
				skeletons[curr_frame_num].bones[0].dof[1] = skeletons[curr_frame_num].bones[0].dof[1] / 180.0*PI;
				skeletons[curr_frame_num].bones[0].dof[2] = skeletons[curr_frame_num].bones[0].dof[2] / 180.0*PI;

			}

			else					        //接下来处理非根节点
			{
				double temp = 0.0;
				while (ss >> temp)
				{
					for (int i = 0; i<3; i++)    //顺着塞进去，只要开关开着的话。当然如果不是按rxryrz顺序就会有bug
					{

						//其余骨架帧都要暂时先借用skeletons[0]的数据：
						//之前复制过了这里就没必要了。
						if (skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dofflag[i] == 1)
						{
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] = temp;
							//AMC数据文件中的数据是角度，这里应该转为弧度，否则后续计算会出错。
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] =
								skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] / 180.0*PI;
							//转换完毕。
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dofflag[i] = 0;	//开关关闭。
							break;
						}	  //End of if.
					}		  //End of for.	
				}		      //End of while.

			}		          //End of else.		
		}		              //End of else. 以上，一行处理完毕。
	}				          //End of while.以上，逐行处理完毕。

}				              //End of Read_AMC.以上，整个AMC文件读取完毕。


							  //定义计算每一帧骨架坐标信息的函数。
void Motion::cal_position()
{
	for (int i = 1; i <= total_frame_num; i++)
	{
		skeletons[i].cal_global_coord();
	}
}


