#include"../Suken.h"

CSystem::CSystem(){
	startTime = 0;
	count = 0;
	fps = 60.0;		//0���Z��h������
	frame = 0;
	escapeFlag = false;
	targetFps = 0.0f;
	screenShotGrHandleAdress = NULL;
	loadingImgPath[0] = NULL;
}
CSystem::~CSystem(){
	
}
void CSystem::Awake(){
	//�Q�[���̒���{�ݒ�A���ʉ���������Ȃ�
	SetWindowIconID( 101 ) ;//�A�C�R���̂��
	SetGraphMode( WINDOW_WIDTH ,  WINDOW_HEIGHT , 32 ) ;//SetWindowSize(WINDOW_WIDTH , WINDOW_HEIGHT );
	ChangeWindowMode(TRUE);
	SetAlwaysRunFlag(TRUE);//�펞�N������̂�TRUE
	SetOutApplicationLogValidFlag( FALSE );//���O�o�͗}������̂�FALSE
	DxLib_Init(); 
	SetDrawScreen( DX_SCREEN_BACK );
	SetTransColor( 255 , 0 , 255 );	//�}�[���^����
	if(loadingImgPath[0] != NULL){
		loadingImg = LoadGraph(loadingImgPath);
	}
	DrawExtendGraph(0,0,WINDOW_WIDTH,WINDOW_HEIGHT,loadingImg,true);
	//���t���b�V�����[�g�̎擾
	HDC hdc;
	hdc = GetDC( GetMainWindowHandle() );//�f�o�C�X�R���e�L�X�g�̎擾
	refreshRate = GetDeviceCaps( hdc , VREFRESH );//���t���b�V�����[�g�̎擾
	ReleaseDC( GetMainWindowHandle() , hdc );//�f�o�C�X�R���e�N�X�g�̉��

	display = CreateDC(TEXT("DISPLAY") , NULL , NULL , NULL);

#ifdef USE_LUA
	Lua = luaL_newstate();
#endif
	ScreenFlip();

	N = refreshRate;//1�b�Ɉ��fps���Z�o

	now = GetNowCount();
	localStandardTime = now;

}
int  CSystem::GetLocalStandardTime(){
	return localStandardTime;
}
void CSystem::SetUseThread_Awake(bool flag){
	useThread_AwakeFlag = flag;
}
bool CSystem::GetUseThread_Awake(){
	return useThread_AwakeFlag;
}
void CSystem::SetLoadingGraph(const char *path){
	strcpy(loadingImgPath,path);
}
void CSystem::SetLoadingMinimalTime(int time){
	loadingMinimalTime = time;
}
void CSystem::Wait_Loading(){
	int restTime = loadingMinimalTime - (GetNowCount()-localStandardTime);
	if(restTime > 0){
		WaitTimer(restTime);
	}
}
void CSystem::Update(){
	//�Վ�
	//N = (int)(GetFps()+0.5);
	//
	now = GetNowCount();
	if( count == 0 ){ //1�t���[���ڂȂ玞�����L��
		startTime = now;
	}
	if( count == N ){ //�K��t���[���ɒB�����畽�ς��v�Z����
		
		fps = 1000/((now-startTime)/(double)N);
		count = 0;
		startTime = now;

	}

	count++;
	frame++;
}
void CSystem::Wait(){
		
#ifdef DEBUG_DRAW
	DrawFormatString(0, 5, WHITE, "%.1f", fps);
#endif
	if(targetFps != 0.0f){
		int tookTime = now - startTime;	//������������
		int waitTime = count*1000/targetFps - tookTime;	//�҂ׂ�����
		if( waitTime > 0 ){
		Sleep(waitTime);	//�ҋ@
		}
	}else{
		Sleep(0);	//�]�����^�C���X���C�X��j��
	}	
}
void CSystem::End(){
	DxLib_End();	
	ShellExecute(GetMainWindowHandle() , "open" , "Launcher.exe" , NULL , NULL , SW_SHOW);
}
void CSystem::TakeScreenShot(){
	if(screenShotFlag){
		screenShotFlag = false;
		if(screenShotGrHandleAdress != NULL){
			*screenShotGrHandleAdress = GetDrawScreen();
		}
		screenShotGrHandleAdress = NULL;
	}
}
void CSystem::GetScreenShot(int *GrHandleAdress){
	screenShotFlag = true;
	screenShotGrHandleAdress = GrHandleAdress;
}
void CSystem::Escape(){
	escapeFlag = true;
}
bool CSystem::GetEscapeFlag(){	
	return escapeFlag;
}
int CSystem::GetFrame(){
	return frame;
}
int CSystem::GetNow(){
	return now;
}
float CSystem::GetFps(){
	return fps;
}
int CSystem::GetRefreshRate(){
	return refreshRate;
}
void CSystem::SetTargetFps(float _fps){
	targetFps = _fps;
}
float CSystem::GetTargetFps(){
	return targetFps;
}
//�V�����X���b�h�����B
void CSystem::CreateNewThread( void(*pFunc)() ){
	if(handleChild.empty()){				
		DxLib::SetMultiThreadFlag( true );			//���̂܂܂ł�DxLib��DirectX�̊֌W�Ń}���`�X���b�h�ɂł��Ȃ��̂Őݒ肵�Ă��K�v������
	}
	//�V�����X���b�h���쐬�i�^�uHANDLE�v�̓|�C���^�Ȃ̂�System::CreateNewThread���\�b�h���I�����Ă������I�ɔj������Ȃ��j
	HANDLE handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)pFunc, NULL, 0, LPDWORD());

	handleChild.push_back(handle);
}
HDC CSystem::GetDisplayDC(){
	return display;
}
#ifdef USE_LUA
lua_State* CSystem::GetLua(){
	return Lua;
}
#endif