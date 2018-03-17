//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  //可去掉控制台窗口
#include "Motion.h"

#define GLUT_DISABLE_ATEXIT_HACK
#define GROUND_SIZE 400.0f

Motion *mptr = new Motion;
static int curr_win;
static int pause = 0;
static int curr_frame_num = 0; //表示当前绘制帧号，在后续绘制中起遍历作用。	

static int glbone = 0;
float color = 0.01;
/*************以下为新全局变量***************************************/
int windowWidth = 800, windowHeight = 600;
double cameraNear = 0.1, cameraFar = 1000.0, cameraFovy = 45.0;
double cameraOR = 100.0, cameraOH = 0.0, cameraOV = 30.0;
int mouseOX, mouseOY;
double cameraR, cameraH, cameraV;
double cameraTR, cameraTH, cameraTV;

int systemPaused = 0, cameraSetting = 0, sliding = 0, cameraTracking = 0;
int trajectory = 0, tBone = 0, showJoints = 0;
int testSwitch = 0;
//Timer fps;
double framerate = 100.0;
float scale = 1.0;
GLubyte boneColor[10][3] = { { 255, 3, 3 },
{ 0, 66, 255 },
{ 28, 230, 185 },
{ 84, 0, 129 },
{ 255, 252, 1 },
{ 125, 125, 125 },  //  {254, 138, 14}
{ 32, 192, 0 },
{ 229, 91, 176 },
{ 97, 52, 5 },
{ 16, 98, 70 } };

GLuint ground, gtex;

float shadowMatrix[4][4];
GLfloat lightPos[] = { 160.0f, 200.0f, 80.0f, 1.0f };
GLfloat groundMat[] = { 0.8f, 0.8f, 0.8f, 0.8f };
GLfloat groundSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat groundShininess[] = { 16.0f };
GLfloat defSpec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat defShininess[] = { 0.0f };

//Posture poseList[MAX_MOVES_IN_AMC_FILE + 1];
//Posture poseList1[MAX_MOVES_IN_AMC_FILE + 1];
//Posture poseListTransform[MAX_MOVES_IN_AMC_FILE + 1];

int poseNum;

//Skeleton sk, skStandard;
int frame;
/*************以上为新全局变量***************************************/

/******************************void computeShadowMatrix(void)**********************************************************/
void computeShadowMatrix(void)
{
	shadowMatrix[0][0] = lightPos[1];
	shadowMatrix[1][0] = -lightPos[0];
	shadowMatrix[2][0] = 0.0f;
	shadowMatrix[3][0] = 0.0f;

	shadowMatrix[0][1] = 0.0f;
	shadowMatrix[1][1] = 0.0f;
	shadowMatrix[2][1] = 0.0f;
	shadowMatrix[3][1] = 0.0f;

	shadowMatrix[0][2] = 0.0f;
	shadowMatrix[1][2] = -lightPos[2];
	shadowMatrix[2][2] = lightPos[1];
	shadowMatrix[3][2] = 0.0f;

	shadowMatrix[0][3] = 0.0f;
	shadowMatrix[1][3] = -1.0f;
	shadowMatrix[2][3] = 0.0f;
	shadowMatrix[3][3] = lightPos[1];
}
/*********************************void initializeGL(void)*******************************************************/
void initializeGL(void)
{
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));
	printf("\n");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	cameraR = cameraOR; cameraH = cameraOH; cameraV = cameraOV;//当前的camera设定为 camere0R

	computeShadowMatrix();

	int i, j;
	GLubyte image[64][64][4];									//设定一个棋盘形状的图片。
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64; j++) {
			if (((i & 0x4) == 0) ^ ((j & 0x4) == 0)) {
				image[i][j][0] = 255;
				image[i][j][1] = 255;
				image[i][j][2] = 255;
			}
			else {
				image[i][j][0] = 128;
				image[i][j][1] = 128;
				image[i][j][2] = 128;
			}
			image[i][j][3] = 255;
		}
	}
	glGenTextures(1, &gtex);
	glBindTexture(GL_TEXTURE_2D, gtex);
	/***********这里的四种纹理参数设置****************************************************************/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	/*******************************************************************************************************/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

/*********************************void initializeLists(void)********************************************************************/
void initializeLists(void)
{
	ground = glGenLists(1);
	glNewList(ground, GL_COMPILE);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gtex);
	/************下面三行代码用于设计地面材质的显示方式******************/
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundMat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, groundShininess);
	/**************************************************************************/
	glPolygonMode(GL_BACK, GL_LINE);					//从下往上可以透视看
	glBegin(GL_QUADS);                                  //画地板
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-0.5f * GROUND_SIZE, 0.0f, 0.5f * GROUND_SIZE);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(0.5f * GROUND_SIZE, 0.0f, 0.5f * GROUND_SIZE);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.5f * GROUND_SIZE, 0.0f, -0.5f * GROUND_SIZE);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-0.5f * GROUND_SIZE, 0.0f, -0.5f * GROUND_SIZE);
	glEnd();
	/*****：上一段代码用于画地板1。设定地板的背面用线画，所以可以从地板下看到地板上，2.将纹理赋予地板*****/
	glPolygonMode(GL_BACK, GL_FILL);                         //骨架成网状模型，与GL_LINE
	glMaterialfv(GL_FRONT, GL_SPECULAR, defSpec);
	glMaterialfv(GL_FRONT, GL_SHININESS, defShininess);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	/***：上一段代码用于设计骨架的现实方式，骨架背面用面显示。***********/
	glEndList();
}
void render(void)
{
	int i;

	if (showJoints) {
		if (cameraV > 0) {
			glColor3ubv(boneColor[0]);
			for (i = 0; i <mptr->skeletons[0].bone_num; i++) {
				glPushMatrix();
				//				glTranslatef(-mptr->skeletons[curr_frame_num].bones[i].dof[0], -mptr->skeletons[curr_frame_num].bones[i].dof[1], mptr->skeletons[curr_frame_num].bones[i].dof[0]);
				glutSolidSphere(0.2, 16, 16);
				glPopMatrix();
			}
		}

		glEnable(GL_BLEND);
		glCallList(ground);
		glDisable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glColor4f(0.0f, 0.0f, 0.0f, 0.25f);
		glPushMatrix();
		glMultMatrixf((GLfloat *)shadowMatrix);
		/*		for (i = 0; i < sk.boneNum; i++) {
		glPushMatrix();
		glTranslatef(poseList[frame].x[i][0],poseList[frame].x[i][1], poseList[frame].x[i][2]);
		glutSolidSphere(0.2, 16, 16);
		glPopMatrix();
		}*/
		glPopMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		if (trajectory) {
			glColor3ubv(boneColor[0]);
			glBegin(GL_LINE_STRIP);
			for (i = 1; i <= frame; i++) {
				//				glVertex3fv(poseList[i].x[tBone]);
			}
			glEnd();
		}

		glColor3ubv(boneColor[0]);
		/*		for (i = 0; i < sk.boneNum; i++) {
		glPushMatrix();
		glTranslatef(poseList[frame].x[i][0],poseList[frame].x[i][1], poseList[frame].x[i][2]);
		glutSolidSphere(0.2, 16, 16);
		glPopMatrix();
		}*/
	}
	else {
		if (cameraV > 0) {
			glPushMatrix();
			glScalef(1.0f, -1.0f, 1.0f);
			glEnable(GL_LIGHTING);
			glEnable(GL_COLOR_MATERIAL);
			glEnable(GL_NORMALIZE);
			glColor3ubv(boneColor[5]);		//变倒影的颜色
											/************************在下面加入代码可以画出地面反射的效果****************************************************************/
											//		glPushAttrib(GL_ALL_ATTRIB_BITS);
											//			glPushMatrix();
											//		glColor3ub(255, 3, 3);
											//		glScalef(1,1,1);
											//		glTranslatef(mptr->skeletons[curr_frame_num].bones[0].global_coord.x,
											//				mptr->skeletons[curr_frame_num].bones[0].global_coord.y,
											//				mptr->skeletons[curr_frame_num].bones[0].global_coord.z);
											//		glTranslatef(-mptr->skeletons[curr_frame_num].bones[0].direc.x*mptr->skeletons[curr_frame_num].bones[0].length,
											//				-mptr->skeletons[curr_frame_num].bones[0].direc.y*mptr->skeletons[curr_frame_num].bones[0].length,
											//				-mptr->skeletons[curr_frame_num].bones[0].direc.z*mptr->skeletons[curr_frame_num].bones[0].length);
											//			glutSolidSphere(mptr->skeletons[curr_frame_num].bones[0].length*0.5, 16, 16);
											//	    glPopMatrix();
											//	for(glbone=1;glbone<mptr->skeletons[0].bone_num-1;glbone++)
											//	{
											//		//vec存储当前的骨骼方向
											//		float vec[3]={0};
											//		int fatherid = mptr->skeletons[curr_frame_num].bones[glbone].father_id;
											//		glPushMatrix();
											//		
											//		glColor3ub(255, 3, 3);
											//				
											//		vec[0] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x;
											//		vec[1] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y;
											//		vec[2] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z;
											//		//上面三行用于获得当前骨骼方向。
											//		glTranslatef( (mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x)/2,
											//				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y)/2,
											//				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z)/2);
											////		将物体旋转至当前坐标系的z轴
											//		glRotatef(180,vec[0]/2.0,vec[1]/2.0,(vec[2] + mptr->skeletons[curr_frame_num].bones[glbone].length)/2.0 );
											//		glScalef(0.3,0.3,1.0);
											//		glutSolidSphere(mptr->skeletons[curr_frame_num].bones[glbone].length*0.5, 16, 16);
											//		glScalef(1,1,1);
											//		glPopMatrix();
											//		}
											//	glPopAttrib();
											/****************************在上面加入代码可以做出地面反射的效果**********************************************************************/
											//			sk.drawPosture(&poseList[frame], SK_BODY);
			glDisable(GL_NORMALIZE);
			glDisable(GL_COLOR_MATERIAL);
			glDisable(GL_LIGHTING);
			glPopMatrix();
		}

		glEnable(GL_BLEND);
		glCallList(ground);
		glDisable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glColor4f(0.0f, 0.0f, 0.0f, 0.25f);
		glPushMatrix();
		glMultMatrixf((GLfloat *)shadowMatrix);
		/***************************************在下面加入代码可以画出阴影*****************************************************/
		//		
		//		glPushAttrib(GL_ALL_ATTRIB_BITS);
		//		glPushMatrix();
		//		glColor3ub(255, 3, 3);
		//		glScalef(1,1,1);
		//		glTranslatef(mptr->skeletons[curr_frame_num].bones[0].global_coord.x,
		//				mptr->skeletons[curr_frame_num].bones[0].global_coord.y,
		//				mptr->skeletons[curr_frame_num].bones[0].global_coord.z);
		//		glTranslatef(-mptr->skeletons[curr_frame_num].bones[0].direc.x*mptr->skeletons[curr_frame_num].bones[0].length,
		//				-mptr->skeletons[curr_frame_num].bones[0].direc.y*mptr->skeletons[curr_frame_num].bones[0].length,
		//				-mptr->skeletons[curr_frame_num].bones[0].direc.z*mptr->skeletons[curr_frame_num].bones[0].length);
		//			glutSolidSphere(mptr->skeletons[curr_frame_num].bones[0].length*0.5, 16, 16);
		//	    glPopMatrix();
		//	for(glbone=1;glbone<mptr->skeletons[0].bone_num-1;glbone++)
		//	{
		//		//vec存储当前的骨骼方向
		//		float vec[3]={0};
		//		int fatherid = mptr->skeletons[curr_frame_num].bones[glbone].father_id;
		//		glPushMatrix();
		//		
		//		glColor3ub(255, 3, 3);
		//				
		//		vec[0] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x;
		//		vec[1] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y;
		//		vec[2] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z;
		//		//上面三行用于获得当前骨骼方向。
		//		glTranslatef( (mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x)/2,
		//				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y)/2,
		//				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z)/2);
		////		将物体旋转至当前坐标系的z轴
		//		glRotatef(180,vec[0]/2.0,vec[1]/2.0,(vec[2] + mptr->skeletons[curr_frame_num].bones[glbone].length)/2.0 );
		//		glScalef(0.3,0.3,1.0);
		//		glutSolidSphere(mptr->skeletons[curr_frame_num].bones[glbone].length*0.5, 16, 16);
		//		glScalef(1,1,1);
		//		glPopMatrix();
		//		}
		//	glPopAttrib();
		/********************************在上面加入画图代码可以画出阴影********************************************************/
		//		sk.drawPosture(&poseList[frame], SK_SHADOW);
		//		skStandard.drawPostureStandard(&poseList1[frame], SK_SHADOW);
		glPopMatrix();
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		/*		if (trajectory) {
		glColor3ubv(boneColor[0]);
		glBegin(GL_LINE_STRIP);
		for (i = 1; i <= frame; i++) {
		glVertex3fv(poseList[i].x[tBone]);
		}
		glEnd();
		}*/

		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_NORMALIZE);
		glEnable(GL_BLEND);
		glColor3ubv(boneColor[5]);
		/*********************************图形代码在下面加入，这里画的图形才具有渲染的效果******************************/
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		glColor3ub(255, 3, 3);
		glScalef(1, 1, 1);
		glTranslatef(mptr->skeletons[curr_frame_num].bones[0].global_coord.x,
			mptr->skeletons[curr_frame_num].bones[0].global_coord.y,
			mptr->skeletons[curr_frame_num].bones[0].global_coord.z);
		glTranslatef(-mptr->skeletons[curr_frame_num].bones[0].direc.x*mptr->skeletons[curr_frame_num].bones[0].length,
			-mptr->skeletons[curr_frame_num].bones[0].direc.y*mptr->skeletons[curr_frame_num].bones[0].length,
			-mptr->skeletons[curr_frame_num].bones[0].direc.z*mptr->skeletons[curr_frame_num].bones[0].length);
		glutSolidSphere(mptr->skeletons[curr_frame_num].bones[0].length*0.5, 16, 16);
		glPopMatrix();
		for (glbone = 1; glbone<mptr->skeletons[0].bone_num - 1; glbone++)
		{
			//vec存储当前的骨骼方向
			float vec[3] = { 0 };
			int fatherid = mptr->skeletons[curr_frame_num].bones[glbone].father_id;
			glPushMatrix();

			glColor3ub(254, 138, 14);

			vec[0] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x;
			vec[1] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y;
			vec[2] = mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z - mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z;
			//上面三行用于获得当前骨骼方向。
			glTranslatef((mptr->skeletons[curr_frame_num].bones[glbone].global_coord.x + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.x) / 2,
				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.y + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.y) / 2,
				(mptr->skeletons[curr_frame_num].bones[glbone].global_coord.z + mptr->skeletons[curr_frame_num].bones[fatherid].global_coord.z) / 2);
			//		将物体旋转至当前坐标系的z轴
			glRotatef(180, vec[0] / 2.0, vec[1] / 2.0, (vec[2] + mptr->skeletons[curr_frame_num].bones[glbone].length) / 2.0);
			glScalef(0.3, 0.3, 1.0);
			glutSolidSphere(mptr->skeletons[curr_frame_num].bones[glbone].length*0.5, 16, 16);
			glScalef(1, 1, 1);
			glPopMatrix();
		}
		glPopAttrib();
		/**********************************画图代码在上面这个部分加入，这里画的图形才具有渲染的效果*******************/
		//	sk.drawPosture(&poseList[frame], SK_BODY);
		//	skStandard.drawPostureStandard(&poseList1[frame], SK_BODY);
		glDisable(GL_BLEND);
		glDisable(GL_NORMALIZE);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
	}
}

//=======================================分割线========================================
//渲染文字到窗口
void render_text()
{
	//解决字体显示的位置等问题，主要是防止上次结果干涉下次。
	glMatrixMode(GL_PROJECTION); //投影模式
	glPushMatrix();              //保留当前投影矩阵
	glLoadIdentity();            //加载单位矩阵，以便后续操作，使之成为当前矩阵
	gluOrtho2D(0, 800, 600, 0);  //左右下上。原点（0,0）位于左上角
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);    //关闭深度测试。

								 //显示文字
	glColor3f(1.0f, 1.0f, 1.0f);							//设置待显示的字符颜色
	char buffer[20];
	sprintf_s(buffer, 20, "Frame: %d / %d", curr_frame_num, mptr->total_frame_num);
	glRasterPos2f(40.0f, 40.0f);							//文字显示位置设置
	for (char* s = buffer; *s != '\0'; s++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);  //逐个绘制图像字符
															//文字显示完毕

	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void display()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -cameraR);
	glRotatef(cameraV, 1.0f, 0.0f, 0.0f);
	glRotatef(-cameraH, 0.0f, 1.0f, 0.0f);

	//打光照
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	//相机追踪
	//	if (cameraTracking)
	//		glTranslatef(-mptr->skeletons[curr_frame_num].bones[0].global_coord.x, -mptr->skeletons[curr_frame_num].bones[0].global_coord.y, -mptr->skeletons[0].bones[0].global_coord.z);

	glMatrixMode(GL_MODELVIEW);

	//改变视角	
	gluLookAt(mptr->skeletons[0].bones[0].global_coord.x - 1.0,
		mptr->skeletons[0].bones[0].global_coord.y - 0.3,
		mptr->skeletons[0].bones[0].global_coord.z,
		mptr->skeletons[0].bones[0].global_coord.x,
		mptr->skeletons[0].bones[0].global_coord.y,
		mptr->skeletons[0].bones[0].global_coord.z,
		0.0,
		1.0,
		0.0);

	render();
	render_text();
	glutSwapBuffers();
}



void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width; windowHeight = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(cameraFovy, (GLdouble)windowWidth / (GLdouble)windowHeight, cameraNear, cameraFar);
	glMatrixMode(GL_MODELVIEW);

	//glLoadIdentity();										//加载单位矩阵
	//*调整视点，将其与根节点绑定。
	/*gluLookAt( mptr->skeletons[curr_frame_num].bones[0].global_coord.x-80.0,
	mptr->skeletons[curr_frame_num].bones[0].global_coord.y,
	mptr->skeletons[curr_frame_num].bones[0].global_coord.z+80.0,
	mptr->skeletons[curr_frame_num].bones[0].global_coord.x-20.0,
	mptr->skeletons[curr_frame_num].bones[0].global_coord.y,
	mptr->skeletons[curr_frame_num].bones[0].global_coord.z+50,
	0.0,
	1.0,
	0.0 ); */
}


void idle(void)
{
	while (!pause)
	{
		if (curr_frame_num < mptr->total_frame_num)
			curr_frame_num++;						 //当前帧号加1
		else curr_frame_num = 1;                     //播放完毕之后循环播放
		glutPostRedisplay();                         //glutPostRedisplay方法将调用display方法进行强制刷新
		break;
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		mptr->Read_AMC("Dushu_Cat3_s.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '2':
		mptr->Read_AMC("Dushu_Crab1_s.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '3':
		mptr->Read_AMC("Dushu_Proud1_s.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '4':
		mptr->Read_AMC("Dushu_Sad_s.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '5':
		mptr->Read_AMC("Dushu_Neutral_s.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '6':
		mptr->Read_AMC("Dushu_Neutral 1.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '7':
		mptr->Read_AMC("Dushu_Tired.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '8':
		mptr->Read_AMC("male.Run9Walk.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case '9':
		mptr->Read_AMC("male.Run10Walk.AMC");
		mptr->cal_position();
		curr_frame_num = 1;
		glutPostRedisplay();
		break;
	case ' ':			 //暂停功能
		pause = (!pause);
		break;
	case 27:             //ESC
		glutDestroyWindow(curr_win);
		exit(0);
		break;
	default:
		break;
	}
}


void specialkey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT://poseNum
		curr_frame_num -= 1;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		curr_frame_num += 1;
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char *argv[])
{

	//以下初始化窗口
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);  //指定RGB&双缓冲窗口模式，要在display()中加入glutSwapBuffers()
	glutInitWindowPosition(100, 100);            //指定窗口起点位置
	glutInitWindowSize(800, 600);                //指定窗口大小
	curr_win = glutCreateWindow("Original Motion");
	//init();                                   //OpenGL状态变量初始化

	initializeGL();								//新函数
	initializeLists();							//新函数

	mptr->Read_ASF("22.asf");
	mptr->Read_AMC("22_01.AMC");
	mptr->cal_position();                       //重要。计算全部帧的各个骨节点的世界坐标

	glutDisplayFunc(display);			        //glutDisplayFunc注册一个回调函数display，相当于对操作系统说：当需要绘制窗口了，请调用display这个函数								        
	glutReshapeFunc(reshape);				    //注册回调函数，响应窗口大小发生改变时的行动
	glutIdleFunc(idle);                         //
	glutKeyboardFunc(keyboard);		            //注册回调函数，响应键盘按键行为	
	glutSpecialFunc(specialkey);				//特殊键值
												//glutMouseFunc(mouse);				    	//注册回调函数，响应鼠标按键行为
	glutMainLoop();                             //正式启动程序，所有绘制的窗口将在这里显示

	delete mptr;						        //程序结束，销毁堆中分配的资源
	return 0;
}



/*
void renderSlideBar(void)
{
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
gluOrtho2D(0, windowWidth, windowHeight, 0);
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
glDisable(GL_DEPTH_TEST);

int px = 100, py = 50, h = 6, r = 8, bx, by;
bx = px + (int)((float)curr_frame_num / (float)mptr->frame_num * (float)(windowWidth - px * 2));
by = windowHeight - py - h / 2;
glEnable(GL_BLEND);
glBegin(GL_QUADS);
//下面的代码绘制进度条
glColor4f(0.0f, 0.2f, 0.5f, 0.8f);
glVertex2i(px, windowHeight - py - h);
glVertex2i(px, windowHeight - py);
glVertex2i(windowWidth - px, windowHeight - py);
glVertex2i(windowWidth - px, windowHeight - py - h);
//上面的代码绘制进度条
//下面代码绘制进度条上的小方块
glColor4f(0.0f, 0.3f, 0.7f, 0.8f);
glVertex2i(bx - r, by - r);
glVertex2i(bx - r, by + r);
glVertex2i(bx + r, by + r);
glVertex2i(bx + r, by - r);
//上面的代码绘制进度条上的小方块
glEnd();
glDisable(GL_BLEND);

glEnable(GL_DEPTH_TEST);
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
glPopMatrix();
}


void renderText(void)
{
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
gluOrtho2D(0, windowWidth, windowHeight, 0);
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
glDisable(GL_DEPTH_TEST);
glColor3f(1.0f, 1.0f, 1.0f);

char buffer[256];

sprintf_s(buffer, 256, "FPS: %.1f", fps.avgHz());
glRasterPos2f(5.0f, 20.0f);
for (char* s = buffer; *s != '\0'; s++)
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);

sprintf_s(buffer, 256, "Time: %.2fs / %.2fs", (float)(curr_frame_num - 1) / 120.0, (float)(mptr->frame_num - 1) / 120.0);
glRasterPos2f(5.0f, 40.0f);
for (char* s = buffer; *s != '\0'; s++)
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);

sprintf_s(buffer, 256, "Frame: %d / %d", curr_frame_num, mptr->frame_num - 1);
glRasterPos2f(5.0f, 60.0f);
for (char* s = buffer; *s != '\0'; s++)
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *s);

glEnable(GL_DEPTH_TEST);
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
glPopMatrix();
}
void drawAxis()
{
glPushMatrix();
glLineWidth(2.0);

glBegin(GL_LINES);
glColor3f(1.0,0.0,0.0);
glVertex3f(0.0,0.0,0.0);
glVertex3f(6.0,0.0,0.0);

glColor3f(0.0,1.0,0.0);
glVertex3f(0.0,0.0,0.0);
glVertex3f(0.0,6.0,0.0);

glColor3f(0.0,0.0,1.0);
glVertex3f(0.0,0.0,0.0);
glVertex3f(0.0,0.0,6.0);
glEnd();

glPopMatrix();
}
*/
//void motion(int x, int y)
//{
//	/*以下函数作用于鼠标拖动进度条时可以定位到某帧开始播放*/
//	if (sliding) {
//		int px = 100, bx;
//		if (x < px) bx = px;
//		else if (x > windowWidth - px) bx = windowWidth - px;
//		else bx = x;
//		curr_frame_num = (float)(bx - px) / (float)(windowWidth - px * 2) *(float)mptr->frame_num;
//		if (curr_frame_num == 0) curr_frame_num = 1;
//		else if (curr_frame_num >= mptr->frame_num) curr_frame_num = mptr->frame_num - 1;
//		glutPostRedisplay();
//	}
//	/*以上函数作用于鼠标拖动进度条时可以定位到某帧开始播放*/
//	else if (cameraSetting == 1) {
//		cameraH = cameraTH - 0.5 * (x - mouseOX);
//		if (cameraH < -360.0) cameraH += 360.0;
//		else if (cameraH > 360.0) cameraH -= 360.0;
//		cameraV = cameraTV + 0.5 * (y - mouseOY);
//		if (cameraV < -90.0 + EPS) cameraV = -90.0 + EPS;
//		else if (cameraV > 90.0 - EPS) cameraV = 90.0 - EPS;
//		glutPostRedisplay();
//	}
//	else if (cameraSetting == 2) {
//		cameraR = cameraTR + (y - mouseOY);
//		if (cameraR < cameraNear) cameraR = cameraNear;
//		else if (cameraR > cameraFar) cameraR = cameraFar;
//		glutPostRedisplay();
//	}
//}

//void iteration(void)
//{
//	glutPostRedisplay();
//	curr_frame_num++;
//	if (curr_frame_num >= mptr->frame_num) {
//		curr_frame_num = 1;
//	}
//	
//}

//void mouse(int button, int state, int x, int y)
//{
//	if (button == GLUT_LEFT_BUTTON) {
//		if (state == GLUT_DOWN) {
//			int px = 100, py = 50, h = 6, r = 8, bx, by;
//			by = windowHeight - py - h / 2;
//			if (y > by - r && y < by + r) {
//				sliding = 1;
//				mouseOX = x; mouseOY = y;
//				if (x < px) bx = px;
//				else if (x > windowWidth - px) bx = windowWidth - px;
//				else bx = x;
//				curr_frame_num = (float)(bx - px) / (float)(windowWidth - px * 2) * (float)mptr->frame_num;
//				if (curr_frame_num == 0) curr_frame_num = 1;
//				else if (curr_frame_num >= mptr->frame_num) frame = mptr->frame_num - 1;
//				glutPostRedisplay();
//			}
//			else if (trajectory) {
////				int tempBone = sk.select(&poseList[frame], x, y);
////				if (tempBone > -1) tBone = tempBone;
//				glutPostRedisplay();
//			}
//		}
//		else if (state == GLUT_UP) {
//			if (sliding) sliding = 0;
//		}
//	}
//	else if (button == GLUT_RIGHT_BUTTON) {
//		if (state == GLUT_DOWN) {
//			cameraSetting = 1;
//			mouseOX = x; mouseOY = y;
//			cameraTH = cameraH; cameraTV = cameraV;
//		}
//		else if (state == GLUT_UP) {
//			cameraSetting = 0;
//		}
//	}
//	else if (button == GLUT_MIDDLE_BUTTON) {
//		if (state == GLUT_DOWN) {
//			cameraSetting = 2;
//			mouseOX = x; mouseOY = y;
//			cameraTR = cameraR;
//		}
//		else if (state == GLUT_UP) {
//			cameraSetting = 0;
//		}
//	}
//}