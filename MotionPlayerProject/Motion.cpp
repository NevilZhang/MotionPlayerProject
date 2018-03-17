#include "Motion.h"
//==========================������Motion���Ա�����Ķ���==================================//
//����Ĭ�Ϲ��캯����
Motion::Motion()
{
	total_frame_num = 0; //�˶�Ƭ����֡��
}

//�����Ա��������ȡASF�ļ���
void Motion::Read_ASF(string filename)
{

	//������������ȡ�ļ�
	ifstream data(filename);
	if (!data) cerr << "asf�ļ���ʧ��!" << endl;

	string line = "0";
	string key_words = "0";		    //���ڱ���ؼ���
	string	tag = "0";			    //tag���Ա�����������״̬
									//���ȴ�����ڵ㣬��ʼ��	
	skeletons[0].bones[0].id = 0;
	skeletons[0].bones[0].name = "root";
	skeletons[0].bones[0].length = 0.0;
	skeletons[0].name_to_id["root"] = 0;
	//���ϣ����ڵ㴦�����

	//��ѭ�������������ļ������ж���
	while (getline(data, line))
	{
		stringstream ss(line);
		ss >> key_words;
		//��������״̬���
		if (key_words == ":root")      tag = ":root";
		if (key_words == ":bonedata")  tag = ":bonedata";
		if (key_words == ":hierarchy") tag = ":hierarchy";
		//���ˣ�״̬����������	
		//����ͳ������ֻ������������һ���Ѿ���ֵ��Ԫ�أ�������ʹ����������ʼ�����Ϊi��Ԫ�ء�
		//����Ҫ���������״����һ��Ԫ��ʱ��ͨ��Ҫʹ�ó�Ա����push_back();���Ը��ö��������ˡ�

		if (key_words == "position" && tag == ":root")      //Ŀǰ�������˸��ڵ�����
		{
			ss >> skeletons[0].bones[0].global_coord.x;
			ss >> skeletons[0].bones[0].global_coord.y;
			ss >> skeletons[0].bones[0].global_coord.z;
		}

		if (key_words == "orientation" && tag == ":root")   //Ŀǰ�������˸��ڵ��ʼ�ֲ�����ϵ
		{
			ss >> skeletons[0].bones[0].local_coord_axis.x;
			ss >> skeletons[0].bones[0].local_coord_axis.y;
			ss >> skeletons[0].bones[0].local_coord_axis.z;
			//ASF�����ļ��е������ǽǶȣ�����Ӧ��תΪ���ȣ����������������
			skeletons[0].bones[0].local_coord_axis.x = skeletons[0].bones[0].local_coord_axis.x / 180.0*PI;
			skeletons[0].bones[0].local_coord_axis.y = skeletons[0].bones[0].local_coord_axis.y / 180.0*PI;
			skeletons[0].bones[0].local_coord_axis.z = skeletons[0].bones[0].local_coord_axis.z / 180.0*PI;

		}
		//���ϣ����ڵ㴦����ϡ�
		//ע������ؼ�����ʱ�Զ����ԡ�

		//���´���:bonedata״̬�£�����������Ҫ����ͬ�Ĺ���������
		if (key_words == "begin" && tag == ":bonedata")      //Ŀǰ����ʽ��ʼ��������
		{
			skeletons[0].bone_num++;
		}

		if (key_words == "id" && tag == ":bonedata")         //Ŀǰ�������˹���id
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].id;
			//����һ����⣬��ǰ������Ӧ�����ڵ�ǰ����id�����򱨴�
			if (skeletons[0].bone_num != skeletons[0].bones[skeletons[0].bone_num].id)
				cerr << "There must be Errors!" << endl;
		}

		if (key_words == "name" && tag == ":bonedata")       //Ŀǰ�������˹���name
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].name;
			skeletons[0].name_to_id[skeletons[0].bones[skeletons[0].bone_num].name] =
				skeletons[0].bones[skeletons[0].bone_num].id; //�����ͨ��map���󶨹������ֺ�id
		}

		if (key_words == "direction" && tag == ":bonedata")  //Ŀǰ�������˹�����ʼdirection
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.x;
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.y;
			ss >> skeletons[0].bones[skeletons[0].bone_num].direc.z;
		}

		if (key_words == "length" && tag == ":bonedata")     //Ŀǰ�������˹���length
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].length;
		}

		if (key_words == "axis" && tag == ":bonedata")       //Ŀǰ�������˹���axis�����ֲ�������
		{
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x;
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y;
			ss >> skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z;
			//ASF�����ļ��е������ǽǶȣ�����Ӧ��תΪ���ȣ����������������
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.x / 180.0*PI;
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.y / 180.0*PI;
			skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z =
				skeletons[0].bones[skeletons[0].bone_num].local_coord_axis.z / 180.0*PI;
		}

		if (key_words == "dof" && tag == ":bonedata")        //Ŀǰ�������˹���dof�������ɶ�
		{
			string temp = "0";
			while (ss >> temp)
			{
				if (temp == "rx")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[0] = 1;     //�൱�ڴ򿪿���
				if (temp == "ry")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[1] = 1;		//�൱�ڴ򿪿���
				if (temp == "rz")
					skeletons[0].bones[skeletons[0].bone_num].dofflag[2] = 1;		//�൱�ڴ򿪿���
			}
		}
		//ע������ֱ�����ӵ�limits��...��rz...

		//���´���:hierarchy״̬�£�
		//�������ݣ�root lhipjoint rhipjoint lowerback
		//          thorax lowerneck lclavicle rclavicle

		if (key_words != ":hierarchy" &&key_words != "begin"&& tag == ":hierarchy")
		{
			if (key_words == "end") break;				//��ʱ��ζ�Ŷ������������ݵĽ�β��
			string temp;
			while (ss >> temp)                         //���л��У���һֱ��
			{
				//�����£�1�����ؼ��ֽڵ���ӵ������ڵ�ĸ��ڵ� 2���������ڵ��Ϊ�ؼ��ֽڵ���ӽڵ�
				skeletons[0].bones[skeletons[0].name_to_id[key_words]].child_id.push_back(skeletons[0].name_to_id[temp]);
				skeletons[0].bones[skeletons[0].name_to_id[temp]].father_id = skeletons[0].name_to_id[key_words];
			}
		}											//End of if

	}												//End of while(getline(data,line))

													//���ر���
	data.close();

}													//End of Skeleton::Read_ASF(char *filename)


													//����AMC�ļ���ȡ���������Դ洢ȫ��֡���ݡ�
void Motion::Read_AMC(string filename)
{

	//���ȴ�������
	ifstream data(filename);
	if (!data) cerr << "amc�ļ���ȡʧ��!" << endl;

	int curr_frame_num = 0;   //���Ա�������ȡ����������֡����Ҫ��main����ļ������֡�
	total_frame_num = 0;      //ÿ�ζ�ȡ����ɽ��֡������˶��л���֡���ٵ��˶������������ǰ�˶������⡣
	string line = "0";
	while (getline(data, line))      //��ѭ�������������ļ������ж���
	{
		//��ȡһ�еĵ�һ������
		string key_words = "0";
		stringstream ss(line);
		ss >> key_words;

		//��һ���߼���Ӧ���������ģ���������һ�У���ͬ�ȶԴ������Թؼ�����Ҫ����һ���ж�:
		//��������֣���ǰ֡״̬����Ϊ�����֣�ֱ��������һ������״̬�Żᷢ���ı�

		//������Ӧ֡�����þ�̬���ݳ�Աmap��������ֵ������Ӧ������dof[3]����Ȼ��Ҫ���ǰ���dofflag[]�����жϡ�
		if (isdigit(key_words[0]))					 //��ʾ��ǰ�������С�������
		{
			//���Ȼ�Ҫ���ַ���ת��Ϊ����
			istringstream temp_string;               //��ʱ�ַ�������ת��
			temp_string.str(key_words);
			temp_string >> curr_frame_num;             //��������µ�ǰ֡��

			skeletons[curr_frame_num] = skeletons[0];  //*��skeletons[0]�ĹǼ����ݸ��Ƹ���ǰ֡�ĹǼ����ݡ�
													   //��Ϊ����֡����һ���Ǽܡ�
													   //�������֡��Ŀ��
			if (curr_frame_num > total_frame_num)
				total_frame_num = curr_frame_num;

		}
		else
		{
			//�������ɶ����ݣ����������Ƹ��ڵ�ֲ�����ϵ��ת�ĽǶ�
			//��Ҫ�Ѹ��ڵ㵥�����������������ڵ��ȫ������Ƚ����⣬ֱ�����ˣ�������㡣
			if (key_words == "root")
			{
				ss >> skeletons[curr_frame_num].bones[0].global_coord.x;
				ss >> skeletons[curr_frame_num].bones[0].global_coord.y;
				ss >> skeletons[curr_frame_num].bones[0].global_coord.z;

				ss >> skeletons[curr_frame_num].bones[0].dof[0];
				ss >> skeletons[curr_frame_num].bones[0].dof[1];
				ss >> skeletons[curr_frame_num].bones[0].dof[2];
				//AMC�����ļ��е������ǽǶȣ�����Ӧ��תΪ���ȣ����������������
				skeletons[curr_frame_num].bones[0].dof[0] = skeletons[curr_frame_num].bones[0].dof[0] / 180.0*PI;
				skeletons[curr_frame_num].bones[0].dof[1] = skeletons[curr_frame_num].bones[0].dof[1] / 180.0*PI;
				skeletons[curr_frame_num].bones[0].dof[2] = skeletons[curr_frame_num].bones[0].dof[2] / 180.0*PI;

			}

			else					        //����������Ǹ��ڵ�
			{
				double temp = 0.0;
				while (ss >> temp)
				{
					for (int i = 0; i<3; i++)    //˳������ȥ��ֻҪ���ؿ��ŵĻ�����Ȼ������ǰ�rxryrz˳��ͻ���bug
					{

						//����Ǽ�֡��Ҫ��ʱ�Ƚ���skeletons[0]�����ݣ�
						//֮ǰ���ƹ��������û��Ҫ�ˡ�
						if (skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dofflag[i] == 1)
						{
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] = temp;
							//AMC�����ļ��е������ǽǶȣ�����Ӧ��תΪ���ȣ����������������
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] =
								skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dof[i] / 180.0*PI;
							//ת����ϡ�
							skeletons[curr_frame_num].bones[skeletons[curr_frame_num].name_to_id[key_words]].dofflag[i] = 0;	//���عرա�
							break;
						}	  //End of if.
					}		  //End of for.	
				}		      //End of while.

			}		          //End of else.		
		}		              //End of else. ���ϣ�һ�д�����ϡ�
	}				          //End of while.���ϣ����д�����ϡ�

}				              //End of Read_AMC.���ϣ�����AMC�ļ���ȡ��ϡ�


							  //�������ÿһ֡�Ǽ�������Ϣ�ĺ�����
void Motion::cal_position()
{
	for (int i = 1; i <= total_frame_num; i++)
	{
		skeletons[i].cal_global_coord();
	}
}


