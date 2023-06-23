#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"//数据库的库，自定义用“”，
                    //sqlite3.c和sqlite3.h文件需要放到该文件同路径下
                    //gcc lianxi.c sqlite3.c -o lianxi -lpthread -ldl
char *LoginUser;//登录用户名
//登陆窗口的全局声明
	GtkWidget *win_login;  //登录窗口
	GtkWidget *label_hint_login;//登录窗口中提示标签
	GtkWidget *entry_acct;  //登录窗口用户行编辑
	GtkWidget *entry_pw ;  //登录窗口密码行编辑 
//注册窗口的全局声明
	GtkWidget *win_reg;  //注册窗口
	GtkWidget *label_hint_reg;//注册窗口中提示标签
	GtkWidget *entry_acct_reg;  //注册窗口用户行编辑
	GtkWidget *entry_pw_reg ;  //注册窗口密码行编辑 
//拼图窗口的相关全局声明
	GtkWidget *win_J;  //拼图窗口
	GtkWidget *label_hint_J;//拼图窗口中提示标签
	GtkWidget *label_hint_count;//记录步数标签
	int PV=0;//预览窗口判断0关 1开
	GtkWidget *img_preview;//预览按钮图
	GtkWidget * img_success;//成功后 遮盖图片控件
	//九宫格图片控件
	GtkWidget * img_J11; GtkWidget * img_J12; GtkWidget * img_J13;
	GtkWidget * img_J21; GtkWidget * img_J22; GtkWidget * img_J23;
	GtkWidget * img_J31; GtkWidget * img_J32; GtkWidget * img_J33;
	int Img_a=0;//切图序号
	//九宫格图片路径，正确
	const char *Original[3][3][3]={"image/1/J11.jpg","image/1/J12.jpg","image/1/J13.jpg",
	                            "image/1/J21.jpg","image/1/J22.jpg","image/1/J23.jpg",
	                            "image/1/J31.jpg","image/1/J32.jpg","image/switch.png",
	                            "image/2/CS_01.jpg","image/2/CS_02.jpg","image/2/CS_03.jpg",
	                            "image/2/CS_04.jpg","image/2/CS_05.jpg","image/2/CS_06.jpg",
	                            "image/2/CS_07.jpg","image/2/CS_08.jpg","image/switch.png",
	                            "image/3/SH_01.jpg","image/3/SH_02.jpg","image/3/SH_03.jpg",
	                            "image/3/SH_04.jpg","image/3/SH_05.jpg","image/3/SH_06.jpg",
	                            "image/3/SH_07.jpg","image/3/SH_08.jpg","image/switch.png"};
	//原图路径
	const char *OriginalImg[3]={"image/1/DrStone.jpg","image/2/CardcaptorSakura.png","image/3/ShadowsHouse.jpg"};
	//当前九宫格路径，设值空白图片，防止误调报错
	const char *tu[3][3]={"image/switch.png","image/switch.png","image/switch.png",
	                      "image/switch.png","image/switch.png","image/switch.png",
	                      "image/switch.png","image/switch.png","image/switch.png"};
	int null_x=2;//初始空白图片横 0-2
	int null_y=2;//初始空白图片竖0-2
	int count=0;//记录步数
    void chaos();
//预览窗口的相关全局声明
	GtkWidget *win_PV;//预览窗口
	GtkWidget *img_bg_PV;//预览窗口背景图
//排行榜窗口的相关全局声明
	GtkWidget *win_rank;//排行榜窗口
	GtkWidget *label_hint_0;GtkWidget *label_hint_1;
	GtkWidget *label_hint_2;GtkWidget *label_hint_3;
	GtkWidget *label_hint_4;GtkWidget *label_hint_5;
	GtkWidget *label_hint_6;GtkWidget *label_hint_7;
	GtkWidget *label_hint_8;GtkWidget *label_hint_9;
	GtkWidget *label_hint_10;GtkWidget *label_hint_11;
//数据库查询循环的相关全局声明
	char **table=NULL;//查看结果表的定义
	int r,c,i,j; //定义查询结果表的行r，列c，循环值判断i和j
	sqlite3 *db=NULL;//定义句柄
	char * errmsg=NULL;//语句错误报错
//颜色调用
int sungtk_color_get(const char *color_buf, GdkColor *color)
{
    gdk_color_parse(color_buf, color);
    return 0;
}
//控件字体颜色设置
int sungtk_widget_set_font_color(GtkWidget *widget, const char *color_buf, gboolean is_button)
{
    if(widget == NULL && color_buf==NULL)
        return -1;
    
    GdkColor color;
    GtkWidget *labelChild = NULL;
    sungtk_color_get(color_buf, &color);
    if(is_button == TRUE){
        labelChild = gtk_bin_get_child(GTK_BIN(widget));//取出GtkButton里的label  
        gtk_widget_modify_fg(labelChild, GTK_STATE_NORMAL, &color);
        gtk_widget_modify_fg(labelChild, GTK_STATE_SELECTED, &color);
        gtk_widget_modify_fg(labelChild, GTK_STATE_PRELIGHT, &color);
    }else{
        gtk_widget_modify_fg(widget, GTK_STATE_NORMAL, &color);
    }
    return 0;
}
//控件字体大小设置
int sungtk_widget_set_font_size(GtkWidget *widget, int size, gboolean is_button)
{
    GtkWidget *labelChild;  
    PangoFontDescription *font;  
    gint fontSize = size;  
    if(widget == NULL)
        return -1;
  
    font = pango_font_description_from_string("Sans");          //"Sans"字体名   
    pango_font_description_set_size(font, fontSize*PANGO_SCALE);//设置字体大小   
    
    if(is_button){
        labelChild = gtk_bin_get_child(GTK_BIN(widget));//取出GtkButton里的label  
    }else{
        labelChild = widget;
    }
    
    //设置label的字体，这样这个GtkButton上面显示的字体就变了
    gtk_widget_modify_font(GTK_WIDGET(labelChild), font);
    pango_font_description_free(font);

    return 0;
}
//图片相关
void load_image(GtkWidget *image, const char *file_path, const int w, const int h )
{
    gtk_image_clear( GTK_IMAGE(image) ); // 清除图像
    GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(file_path, NULL); // 创建图片资源
    GdkPixbuf *dest_pixbuf=gdk_pixbuf_scale_simple(src_pixbuf,w,h,GDK_INTERP_BILINEAR); // 指定大小
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), dest_pixbuf); // 图片控件重新设置一张图片(pixbuf)
    g_object_unref(src_pixbuf); // 释放资源
    g_object_unref(dest_pixbuf); // 释放资源
}

void database()
{
    int ret = sqlite3_open("heng.db",&db);//打开数据库，放入句柄
    if(ret!=SQLITE_OK)//判断打开失败
    {
        perror("sqlite3_open"); //打印错误原因
        gtk_main_quit;
    }
    //创建数据表  用户表user    acct>account>账户   pw>password>密码
    sqlite3_exec(db,"create table user(acct text,pw text);",NULL,NULL,&errmsg);
    // 排行榜表rank    acct>账户       bu>成功的步数
    sqlite3_exec(db,"create table rank(acct text,bu int);",NULL,NULL,&errmsg);
}
//根据账户查询
void AcctQuery(const gchar *acct) 
{
	database();
    char cmd1[100]="";
    sprintf(cmd1,"select * from user where acct='%s';",acct);//组包 
                                                //数据库根据acct查询语句
    errmsg=NULL;
    sqlite3_get_table(db,cmd1,&table,&r,&c,&errmsg);
    if (errmsg!=NULL)  //测试数据库语句是否正确
    {
     gtk_label_set_text(GTK_LABEL(label_hint_login),errmsg);
     r=-1;
    }
    sqlite3_close(db);
}
//根据账户和密码查询
void AAndPQuery(const gchar *acct,const gchar *pw) 
{
	database();
    char cmd1[100]="";
    sprintf(cmd1,"select * from user where acct='%s' and pw='%s';",acct,pw);//组包 
                                                //数据库根据acct查询语句
    errmsg=NULL;
    sqlite3_get_table(db,cmd1,&table,&r,&c,&errmsg);
    if (errmsg!=NULL)  //测试数据库语句是否正确
    {
     gtk_label_set_text(GTK_LABEL(label_hint_login),errmsg);
     r=-1;
    }
    sqlite3_close(db);
}
//添加用户数据
void add(const gchar *acct,const gchar *pw)
{ 
	database();
    char cmd[100]="";
    sprintf(cmd,"insert into user values('%s','%s');",acct,pw);//组包 
                                                         //数据库添加语句
    errmsg=NULL;
    sqlite3_exec(db,cmd,NULL,NULL,&errmsg);//发包  
    // if (errmsg!=NULL) //测试数据库语句是否正确
    // {
    //     gtk_label_set_text(GTK_LABEL(label_hint_reg),errmsg);
    // }
    // else
    // {
       gtk_label_set_text(GTK_LABEL(label_hint_reg),"注册成功");
    // }
    sqlite3_close(db);
}
void LoginToJigsaw()//登陆界面跳转到拼图界面
{
	gtk_label_set_text(GTK_LABEL(label_hint_login),"");
	gtk_widget_hide_all(win_login);//隐藏
    gtk_widget_show_all(win_J);//显示
    chaos(); 
}
void MyLogin()//登录功能
{
    const gchar *text_acct=gtk_entry_get_text(GTK_ENTRY(entry_acct));//获取登录界面账号
    const gchar *text_pw=gtk_entry_get_text(GTK_ENTRY(entry_pw));//获取登录界面密码

    if( strcmp(text_acct,"")==0 || strcmp(text_pw,"")==0 ){
        gtk_label_set_text(GTK_LABEL(label_hint_login),"账户、密码不能为空，请重新输入");
    }
    else{
        AcctQuery(text_acct);
        if(r==0){
            gtk_label_set_text(GTK_LABEL(label_hint_login),"账户不存在,请重新输入");
        }else{
            AAndPQuery(text_acct,text_pw);
            if(r==0){
                 gtk_label_set_text(GTK_LABEL(label_hint_login),"账户或密码错误,请重新输入");
            }
            else{
                LoginUser = table[2];
            	//LoginUser = table[3];//2才是第一个用户3是密码
                //printf("%s\n",LoginUser);
                //gtk_label_set_text(GTK_LABEL(label_hint_login),"登录成功，等待跳转");
                LoginToJigsaw();
            }
        }     
    }
    gtk_entry_set_text(GTK_ENTRY(entry_acct),"");
    gtk_entry_set_text(GTK_ENTRY(entry_pw),"");
}

void ToWinReg()//跳转到注册窗口
{  
	gtk_label_set_text(GTK_LABEL(label_hint_login),"");
    gtk_widget_hide_all(win_login);//隐藏
    gtk_widget_show_all(win_reg);//显示

}
void WinLogin() //登录窗口
{
	//************创建登录窗口**********************************************
	win_login = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(win_login,800,450);
	gtk_window_set_title(GTK_WINDOW(win_login),"登录界面");
	g_signal_connect(win_login,"destroy",G_CALLBACK(gtk_main_quit),NULL);

    //**************创建固定布局*****************************************
	GtkWidget *fixed =gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win_login),fixed);

    //*****************界面背景****************************************
    GtkWidget * img_bg=gtk_image_new_from_pixbuf(NULL);
    load_image(img_bg,"image/login1.jpg",800,450);
    gtk_fixed_put(GTK_FIXED(fixed),img_bg,0,0);

    //********************提示标签**************************************
    label_hint_login =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_login ,80,50);
     sungtk_widget_set_font_color(label_hint_login ,"#ff3b3b",FALSE);
    sungtk_widget_set_font_size(label_hint_login ,20,FALSE);

    //************账号行*****************************************
    GtkWidget *lbl_acct =gtk_label_new("账户:"); //
    gtk_fixed_put(GTK_FIXED(fixed),lbl_acct,90,150);
    sungtk_widget_set_font_color(lbl_acct,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(lbl_acct,30,FALSE);
    entry_acct =gtk_entry_new(); //行编辑
    gtk_fixed_put(GTK_FIXED(fixed),entry_acct,200,150);
    sungtk_widget_set_font_size(entry_acct,20,FALSE);
    gtk_widget_set_size_request(entry_acct,200,50);


    //*******************密码行********************************
    GtkWidget *label_pw =gtk_label_new("密码:");
    gtk_fixed_put(GTK_FIXED(fixed),label_pw,90,230);
    sungtk_widget_set_font_color(label_pw,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_pw,30,FALSE);
    entry_pw =gtk_entry_new();  //行编辑
    gtk_fixed_put(GTK_FIXED(fixed),entry_pw,200,230);
    sungtk_widget_set_font_size(entry_pw,20,FALSE);
    gtk_widget_set_size_request(entry_pw,200,50);


    //*****************登录按钮 login****************************************
    GtkWidget *btn_login =gtk_button_new_with_label("登录");
    //gtk_widget_set_size_request(btn_login,100,45);
    gtk_fixed_put(GTK_FIXED(fixed),btn_login,120,340);
    sungtk_widget_set_font_color(btn_login,"#FF00FF",TRUE);
   sungtk_widget_set_font_size(btn_login,35,TRUE);
    gtk_button_set_relief(GTK_BUTTON(btn_login), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_login,"pressed",G_CALLBACK(MyLogin),NULL);//点击事件

    //*****************注册按钮 reg****************************************
    GtkWidget *btn_reg =gtk_button_new_with_label("注册");
    gtk_fixed_put(GTK_FIXED(fixed),btn_reg,280,340);
    sungtk_widget_set_font_color(btn_reg,"#FF00FF",TRUE);
    sungtk_widget_set_font_size(btn_reg,35,TRUE);
    gtk_button_set_relief(GTK_BUTTON(btn_reg), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_reg,"pressed",G_CALLBACK(ToWinReg),NULL);//点击事件
}
void MyRegister()//注册页面按钮功能
{
    const gchar *text_acct_reg=gtk_entry_get_text(GTK_ENTRY(entry_acct_reg));
    const gchar *text_pw_reg=gtk_entry_get_text(GTK_ENTRY(entry_pw_reg));
    if(strcmp(text_acct_reg,"")==0 || strcmp(text_pw_reg,"")==0 ){
        gtk_label_set_text(GTK_LABEL(label_hint_reg),"账户、密码不能为空，请重新输入");
    }
    else{
        AcctQuery(text_acct_reg);
        if(r!=0){
            gtk_label_set_text(GTK_LABEL(label_hint_reg),"账户已存在,请重新输入");
        }else {
            add(text_acct_reg,text_pw_reg);
        }     
    }
    gtk_entry_set_text(GTK_ENTRY(entry_acct_reg),"");
    gtk_entry_set_text(GTK_ENTRY(entry_pw_reg),"");
}
void ToWinLogin()//跳转到登录窗口
{
	gtk_label_set_text(GTK_LABEL(label_hint_reg),"");
    gtk_widget_hide_all(win_reg);//隐藏
    gtk_widget_show_all(win_login);//显示
}
void WinReg() //注册窗口
{
    //************创建注册窗口**********************************************
    win_reg = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(win_reg,800,450);
    gtk_window_set_title(GTK_WINDOW(win_reg),"注册界面");
    g_signal_connect(win_reg,"destroy",G_CALLBACK(gtk_main_quit),NULL);

    //**************创建固定布局*****************************************
    GtkWidget *fixed =gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win_reg),fixed);

    //*****************界面背景****************************************
    GtkWidget * img_bg=gtk_image_new_from_pixbuf(NULL);
    load_image(img_bg,"image/register1.jpg",800,450);
    gtk_fixed_put(GTK_FIXED(fixed),img_bg,0,0);

    //********************提示标签**************************************
    label_hint_reg =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_reg ,350,50);
     sungtk_widget_set_font_color(label_hint_reg ,"#ff3b3b",FALSE);
    sungtk_widget_set_font_size(label_hint_reg ,20,FALSE);

    //************账号行*****************************************
    GtkWidget *lbl_acct_reg =gtk_label_new("账户:"); //
    gtk_fixed_put(GTK_FIXED(fixed),lbl_acct_reg,400,150);
    sungtk_widget_set_font_color(lbl_acct_reg,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(lbl_acct_reg,30,FALSE);
    entry_acct_reg =gtk_entry_new(); //行编辑
    gtk_fixed_put(GTK_FIXED(fixed),entry_acct_reg,510,150);
    sungtk_widget_set_font_size(entry_acct_reg,20,FALSE);
    gtk_widget_set_size_request(entry_acct_reg,200,50);

    //*******************密码行********************************
    GtkWidget *label_pw_reg =gtk_label_new("密码:");
    gtk_fixed_put(GTK_FIXED(fixed),label_pw_reg,400,230);
    sungtk_widget_set_font_color(label_pw_reg,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_pw_reg,30,FALSE);
    entry_pw_reg =gtk_entry_new();  //行编辑
    gtk_fixed_put(GTK_FIXED(fixed),entry_pw_reg,510,230);
    sungtk_widget_set_font_size(entry_pw_reg,20,FALSE);
    gtk_widget_set_size_request(entry_pw_reg,200,50);

    //*****************登录界面按钮 login****************************************
    GtkWidget *btn_back =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_back,730,0);
    GtkWidget *img_toLogin=gtk_image_new_from_pixbuf(NULL);
    load_image(img_toLogin,"image/return.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_back),img_toLogin);    
    gtk_button_set_relief(GTK_BUTTON(btn_back), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_back,"pressed",G_CALLBACK(ToWinLogin),NULL);//点击事件

    //*****************注册按钮 reg****************************************
    GtkWidget *btn_reg_reg =gtk_button_new_with_label("注册");
    gtk_fixed_put(GTK_FIXED(fixed),btn_reg_reg,530,340);
    sungtk_widget_set_font_color(btn_reg_reg,"#FF00FF",TRUE);
    sungtk_widget_set_font_size(btn_reg_reg,35,TRUE);
    gtk_button_set_relief(GTK_BUTTON(btn_reg_reg), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_reg_reg,"pressed",G_CALLBACK(MyRegister),NULL);//点击事件
}
//*******************************************************


void AddRank ()  //添加排行榜数据
{ 
	database();
	//printf("%s\n",LoginUser);
    char cmd[100]="";
    sprintf(cmd,"insert into rank values('%s',%d);",LoginUser,count);//组包 
                                                         //数据库添加语句
    errmsg=NULL;
    sqlite3_exec(db,cmd,NULL,NULL,&errmsg);//发包  
    sqlite3_close(db);
}

int vicotory()//判断拼图是否成功
{
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(strcmp(tu[i][j],Original[Img_a][i][j])!=0)
            {
                return 0;
            }
        }
    }
    return 1;
}
void SetImg() //根据数组调整图片
{
    load_image(img_J11,tu[0][0],120,120);
    load_image(img_J12,tu[0][1],120,120);
    load_image(img_J13,tu[0][2],120,120);
    load_image(img_J21,tu[1][0],120,120);
    load_image(img_J22,tu[1][1],120,120);
    load_image(img_J23,tu[1][2],120,120);
    load_image(img_J31,tu[2][0],120,120);
    load_image(img_J32,tu[2][1],120,120);
    load_image(img_J33,tu[2][2],120,120);
}
void ResetCount()//重置拼图步数
{
    count=0;
    gtk_label_set_text(GTK_LABEL(label_hint_count),"");
    gtk_label_set_text(GTK_LABEL(label_hint_J),"");
   //load_image(img_bg_PV,OriginalImg[Img_a],240,240);
}
//*******************************************************************************
void success()//成功遮盖按钮功能
{
    gtk_label_set_text(GTK_LABEL(label_hint_J),"请点击图下面的更新按钮或切图按钮开始新游戏");
}
void huan(int x,int y)//更换选中按钮和空白按钮对应数组数据
{
    tu[null_x][null_y]=tu[x][y];
    tu[x][y]="image/switch.png";
    null_x=x;
    null_y=y; 
    SetImg();
    count++;
    char cmd[50]="";
    sprintf(cmd,"已走%d步",count);
    gtk_label_set_text(GTK_LABEL(label_hint_count),cmd);
    gtk_label_set_text(GTK_LABEL(label_hint_J),"");
    int vv=vicotory();
    //printf("%d\n",vv); 
    if (vv==1){
        load_image(img_success,OriginalImg[Img_a],360,360);
        gtk_label_set_text(GTK_LABEL(label_hint_J),"拼图成功，已录入数据");
        AddRank();
    } 
}
//拼图按钮功能 ，判断，与空白相邻互换图片，不相邻提示
void qiehuan(GtkWidget *checked_but , gpointer data)
{
    int btn_data=GPOINTER_TO_INT(data);
    int y=btn_data%10-1;//竖
    int x=(btn_data-y-1)/10-1;//横
    if(x==null_x+1 && y==null_y){ huan(x, y);}
    else if(x==null_x-1 && y==null_y){huan(x, y);}
    else if(x==null_x && y==null_y+1) {huan(x, y);}
    else if(x==null_x && y==null_y-1){ huan(x, y);}
    else if(x==null_x && y==null_y)
        {gtk_label_set_text(GTK_LABEL(label_hint_J),"请点击与空白图片相接的图片");}
    else {gtk_label_set_text(GTK_LABEL(label_hint_J),"该图片不与空白图片相接");}
}
//java的打乱  ≧ ﹏ ≦
/*void chaos(){
    int tempArr[]={0,1,2,3,4,5,6,7,8};
    Random random=new Random();
    for(int i=0;i<tempArr.length;i++){
        int index=random.nextInt(tempArr.length);
        int temp=tempArr[i];
        tempArr[i]=tempArr[index];
        tempArr[index]=temp;
    }
    int ox=0,oy=0;
    for(i=0;i<tempArr.length;i++){
        int n=tempArr[i]%3;
        int m=(tempArr[i]-n)/3;
        tu[m][n]=Original[Img_a][ox][oy];
        oy++;
        if(oy>2){
            oy=0;
            ox++;
        }
    }
    null_x=m;
    null_y=n;
    SetImg();
}*/
void preview()//预览按钮功能
{
    if (PV==0)
    {
        //printf("%d\n", PV);
        gtk_widget_show_all(win_PV);//显示
        load_image(img_preview,"image/preview-open.png",50,50);
        PV=1;
    }
    else if(PV==1)
    {
        //printf("%d\n", PV);
        gtk_widget_hide_all(win_PV);//隐藏
        load_image(img_preview,"image/preview-close.png",50,50);
        PV=0;
    }
}
void SetHintNULL()//排行榜置空
{
	gtk_label_set_text(GTK_LABEL(label_hint_11),"");
	gtk_label_set_text(GTK_LABEL(label_hint_10),"");
	gtk_label_set_text(GTK_LABEL(label_hint_9),"");
	gtk_label_set_text(GTK_LABEL(label_hint_8),"");
	gtk_label_set_text(GTK_LABEL(label_hint_7),"");
	gtk_label_set_text(GTK_LABEL(label_hint_6),"");
	gtk_label_set_text(GTK_LABEL(label_hint_5),"");
	gtk_label_set_text(GTK_LABEL(label_hint_4),"");
	gtk_label_set_text(GTK_LABEL(label_hint_3),"");
	gtk_label_set_text(GTK_LABEL(label_hint_2),"");
	gtk_label_set_text(GTK_LABEL(label_hint_1),"");
	gtk_label_set_text(GTK_LABEL(label_hint_0),"");
}
void ReadRank()//根据数据库显示排行榜
{
	SetHintNULL();
	database();
	sqlite3_get_table(db,"select * from rank order by bu asc;",&table,&r,&c,&errmsg);
	if(r==0)
	{
		gtk_label_set_text(GTK_LABEL(label_hint_0),"暂无数据");
	}
	else
	{
		int rc=(r+1)*c-1;
		if (rc>11){
			rc=11;
		}
		switch(rc){

			case 11:gtk_label_set_text(GTK_LABEL(label_hint_11),table[11]);
					gtk_label_set_text(GTK_LABEL(label_hint_10),table[10]);
			case 9:	gtk_label_set_text(GTK_LABEL(label_hint_9),table[9]);
				   	gtk_label_set_text(GTK_LABEL(label_hint_8),table[8]);
			case 7:	gtk_label_set_text(GTK_LABEL(label_hint_7),table[7]);
					gtk_label_set_text(GTK_LABEL(label_hint_6),table[6]);
			case 5:	gtk_label_set_text(GTK_LABEL(label_hint_5),table[5]);
					gtk_label_set_text(GTK_LABEL(label_hint_4),table[4]);
			case 3:	gtk_label_set_text(GTK_LABEL(label_hint_3),table[3]);
					gtk_label_set_text(GTK_LABEL(label_hint_2),table[2]);
					gtk_label_set_text(GTK_LABEL(label_hint_1),"所用步数");
					gtk_label_set_text(GTK_LABEL(label_hint_0),"用户");
		}
	}
	sqlite3_close(db);
}
void ToRank()//从拼图窗口跳转到排行榜
{
	
	if(PV==1){
		preview();
	}
    gtk_widget_hide_all(win_J);//隐藏
    gtk_widget_show_all(win_rank);//显示
    ReadRank();
}
void chaos()//打乱按钮功能，随机调整图片位置
{
    //*********打乱一个一维数组*******
    int a[] = {0,1,2,3,4,5,6,7,8};
    int len = ( sizeof(a) / sizeof(a[0]) );
    int t;
    srand( time(NULL) );
    int rN1 = (rand() % len);
    int rN2 = (rand() % len);
    for ( i = 0; i < 20; i++) {
        if (rN1 != rN2) {
            t = a[rN1];
            a[rN1] = a[rN2];
            a[rN2] = t;
        }
        rN1 = (rand() % len);
        rN2 = (rand() % len);
    }
    //****根据打乱的一维数组设置图片*******
    int m,n,ox,oy;
    for(i=0;i<len;i++){
        n=a[i]%3;
        m=(a[i]-n)/3;
        ox=i/3;
        oy=i%3;
        tu[m][n]=Original[Img_a][ox][oy];
    }
    null_x=m;//重新确认空白位置
    null_y=n;
    SetImg();//让图片显现
    load_image(img_success,"image/switch.png",1,1);//隐藏遮盖按钮
    ResetCount();
}
void show()//展示按钮功能，让图片可以再走一步复位，方便演示成功情况
{
    tu[0][0]=Original[Img_a][0][0]; tu[0][1]=Original[Img_a][0][1]; tu[0][2]=Original[Img_a][0][2];
    tu[1][0]=Original[Img_a][1][0]; tu[1][1]=Original[Img_a][1][1]; tu[1][2]=Original[Img_a][1][2];
    tu[2][0]=Original[Img_a][2][0]; tu[2][1]=Original[Img_a][2][2]; tu[2][2]=Original[Img_a][2][1];
    null_x=2;
    null_y=1;
    SetImg();
    ResetCount();
    load_image(img_success,"image/switch.png",1,1);
    load_image(img_bg_PV,OriginalImg[Img_a],240,240);
}
void cutover()//切换图片功能
{
    Img_a++;
    if (Img_a>2)
    {
      Img_a=0;
    }
    chaos();
    ResetCount();
    load_image(img_bg_PV,OriginalImg[Img_a],240,240);
}

void ToJigsaw()//从排行榜跳转到拼图窗口
{
	
    gtk_widget_hide_all(win_rank);//隐藏
    gtk_widget_show_all(win_J);//显示
    chaos();
}



//void setPV(){PV=0;}

void WinPreview()//预览图片窗口
{
    //************创建登录窗口**********************************************
    win_PV= gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(win_PV,240,240);
    gtk_window_set_title(GTK_WINDOW(win_PV),"预览图片");
    //g_signal_connect(win_PV,"destroy",G_CALLBACK(setPV),NULL);
    //g_signal_connect(win_PV,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_window_set_deletable(GTK_WINDOW (win_PV), FALSE);//不让有x

    //**************创建固定布局*****************************************
    GtkWidget *fixed =gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win_PV),fixed);

    //*****************界面背景****************************************
    img_bg_PV=gtk_image_new_from_pixbuf(NULL);
    load_image(img_bg_PV,OriginalImg[Img_a],240,240);
    gtk_fixed_put(GTK_FIXED(fixed),img_bg_PV,0,0);
}
void WinJigsaw() //拼图窗口
{
    //************创建拼图窗口**********************************************
    win_J = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(win_J,360,640);
    gtk_window_set_title(GTK_WINDOW(win_J),"拼图界面");
    g_signal_connect(win_J,"destroy",G_CALLBACK(gtk_main_quit),NULL);

    //**************创建固定布局*****************************************
    GtkWidget *fixed =gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win_J),fixed);

    //*****************界面背景****************************************
    GtkWidget * img_bg=gtk_image_new_from_pixbuf(NULL);
    load_image(img_bg,"image/J.jpg",360,640);
    gtk_fixed_put(GTK_FIXED(fixed),img_bg,0,0);

    //********************步数标签**************************************
    label_hint_count =gtk_label_new("已走0步");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_count ,130,50);
    sungtk_widget_set_font_color(label_hint_count ,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_count,25,FALSE);

    //********************提示标签**************************************
    label_hint_J =gtk_label_new("我是提示标签");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_J ,32,100);
    sungtk_widget_set_font_color(label_hint_J ,"#ff3b3b",FALSE);
    sungtk_widget_set_font_size(label_hint_J ,15,FALSE);
//*1*********************J11**********************************
    GtkWidget *btn_J11 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J11,120,120); //不设置按钮大小，防止间隙
    gtk_fixed_put(GTK_FIXED(fixed),btn_J11,0,160);
    gtk_button_set_relief(GTK_BUTTON(btn_J11), GTK_RELIEF_NONE);//把按钮设置为透明
    img_J11=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J11,"image/J11.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J11),img_J11);
    g_signal_connect(btn_J11,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(11));//点击事件
    //*2*********************J12**********************************
    GtkWidget *btn_J12 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J12,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J12,120,160);
    img_J12=gtk_image_new_from_pixbuf(NULL);
    gtk_button_set_relief(GTK_BUTTON(btn_J12), GTK_RELIEF_NONE);//把按钮设置为透明
    //load_image(img_J12,"image/J12.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J12),img_J12);
    
    g_signal_connect(btn_J12,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(12));//点击事件
    //*3*********************J13**********************************
    GtkWidget *btn_J13 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J13,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J13,240,160);
    gtk_button_set_relief(GTK_BUTTON(btn_J13), GTK_RELIEF_NONE);//把按钮设置为透明
    img_J13=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J13,"image/J13.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J13),img_J13);
    
    g_signal_connect(btn_J13,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(13));//点击事件
    //*4*********************J21**********************************
    GtkWidget *btn_J21 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J21,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J21,0,280);
    img_J21=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J21,"image/J21.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J21),img_J21);
    gtk_button_set_relief(GTK_BUTTON(btn_J21), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J21,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(21));//点击事件
    //*5*********************J22**********************************
    GtkWidget *btn_J22 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J22,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J22,120,280);
    img_J22=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J22,"image/J22.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J22),img_J22);
    gtk_button_set_relief(GTK_BUTTON(btn_J22), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J22,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(22));//点击事件
    //*6*********************J23**********************************
    GtkWidget *btn_J23 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J23,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J23,240,280);
    img_J23=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J23,"image/J23.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J23),img_J23);
    gtk_button_set_relief(GTK_BUTTON(btn_J23), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J23,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(23));//点击事件
    //*7*********************J31**********************************
    GtkWidget *btn_J31 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J31,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J31,0,400);
    img_J31=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J31,"image/J31.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J31),img_J31);
    gtk_button_set_relief(GTK_BUTTON(btn_J31), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J31,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(31));//点击事件
    //*8*********************J32**********************************
    GtkWidget *btn_J32 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J32,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J32,120,400);
    img_J32=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J32,"image/J32.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J32),img_J32);
    gtk_button_set_relief(GTK_BUTTON(btn_J32), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J32,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(32));//点击事件
    //*9*********************J33**********************************
    GtkWidget *btn_J33 =gtk_button_new();
    //gtk_widget_set_size_request(btn_J33,120,120);
    gtk_fixed_put(GTK_FIXED(fixed),btn_J33,240,400);
    img_J33=gtk_image_new_from_pixbuf(NULL);
    //load_image(img_J33,"image/J33.jpg",120,120);
    gtk_container_add(GTK_CONTAINER(btn_J33),img_J33);
    gtk_button_set_relief(GTK_BUTTON(btn_J33), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_J33,"pressed",G_CALLBACK(qiehuan),GINT_TO_POINTER(33));//点击事件 
//*****************排行榜 ranking****************************************
    GtkWidget *btn_ranking =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_ranking,25,540);
    GtkWidget *img_ranking=gtk_image_new_from_pixbuf(NULL);
    load_image(img_ranking,"image/ranking.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_ranking),img_ranking);   
    gtk_button_set_relief(GTK_BUTTON(btn_ranking), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_ranking,"pressed",G_CALLBACK(ToRank),NULL);//点击事件 
    //*****************演示按钮 show****************************************
    GtkWidget *btn_show =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_show,90,540);
    GtkWidget *img_show=gtk_image_new_from_pixbuf(NULL);
    load_image(img_show,"image/show.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_show),img_show);
    gtk_button_set_relief(GTK_BUTTON(btn_show), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_show,"pressed",G_CALLBACK(show),NULL);//点击事件
    //*****************打乱按钮 chaos****************************************
    GtkWidget *btn_chaos =gtk_button_new();//创建按钮
    gtk_fixed_put(GTK_FIXED(fixed),btn_chaos,155,540);//
    GtkWidget *img_chaos=gtk_image_new_from_pixbuf(NULL);
    load_image(img_chaos,"image/chaos.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_chaos),img_chaos);
    gtk_button_set_relief(GTK_BUTTON(btn_chaos), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_chaos,"pressed",G_CALLBACK(chaos),NULL);//点击事件
    //*****************切图 cutover****************************************
    GtkWidget *btn_cutover =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_cutover,220,540);
    GtkWidget *img_cutover=gtk_image_new_from_pixbuf(NULL);
    load_image(img_cutover,"image/cutover.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_cutover),img_cutover);    
    gtk_button_set_relief(GTK_BUTTON(btn_cutover), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_cutover,"pressed",G_CALLBACK(cutover),NULL);//点击事件
    //*****************预览 preview****************************************
    GtkWidget *btn_preview =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_preview,285,540);
    img_preview=gtk_image_new_from_pixbuf(NULL);
    load_image(img_preview,"image/preview-close.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_preview),img_preview);   
    gtk_button_set_relief(GTK_BUTTON(btn_preview), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_preview,"pressed",G_CALLBACK(preview),NULL);//点击事件  

    //********************成功遮盖**********************
    GtkWidget *btn_success =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_success,0,159);
    gtk_button_set_relief(GTK_BUTTON(btn_success), GTK_RELIEF_NONE);
    img_success=gtk_image_new_from_pixbuf(NULL);
    gtk_container_add(GTK_CONTAINER(btn_success),img_success);
    //g_signal_connect(btn_preview,"pressed",G_CALLBACK(success),NULL);//点击事件  
    //**************
    
}


void WinRanking()//排行榜窗口
{
    //************创建排行榜窗口**********************************************
    win_rank = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(win_rank,360,640);
    gtk_window_set_title(GTK_WINDOW(win_rank),"拼图界面");
    g_signal_connect(win_rank,"destroy",G_CALLBACK(gtk_main_quit),NULL);

    //**************创建固定布局*****************************************
    GtkWidget *fixed =gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(win_rank),fixed);

    //*****************界面背景****************************************
    GtkWidget * img_bg=gtk_image_new_from_pixbuf(NULL);
    load_image(img_bg,"image/rank.jpg",360,640);
    gtk_fixed_put(GTK_FIXED(fixed),img_bg,0,0);

    //*****************返回 toJigsaw****************************************
    GtkWidget *btn_toJigsaw =gtk_button_new();
    gtk_fixed_put(GTK_FIXED(fixed),btn_toJigsaw,300,0);
    GtkWidget *img_toJigsaw=gtk_image_new_from_pixbuf(NULL);
    load_image(img_toJigsaw,"image/return.png",50,50);
    gtk_container_add(GTK_CONTAINER(btn_toJigsaw),img_toJigsaw);    
    gtk_button_set_relief(GTK_BUTTON(btn_toJigsaw), GTK_RELIEF_NONE);//把按钮设置为透明
    g_signal_connect(btn_toJigsaw,"pressed",G_CALLBACK(ToJigsaw),NULL);//点击事件
    //********************总排行榜标签**************************************
    GtkWidget *label_hint =gtk_label_new("总排行榜");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint ,50,50);
    sungtk_widget_set_font_color(label_hint ,"#07FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint,50,FALSE);
    //**********************用户标签*****************************
    label_hint_0 =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_0 ,60,150);
    sungtk_widget_set_font_color(label_hint_0 ,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_0,30,FALSE);
    //**********************步数标签*****************************
    label_hint_1 =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_1 ,170,150);
    sungtk_widget_set_font_color(label_hint_1 ,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_1,30,FALSE);
    //**********************2偶数标签*****************************
    label_hint_2 =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_2 ,70,200);
    sungtk_widget_set_font_color(label_hint_2,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_2,30,FALSE);
    //**********************3奇数标签*****************************
    label_hint_3 =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_3 ,250,200);
    sungtk_widget_set_font_color(label_hint_3,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_3,30,FALSE);
   //**********************4偶数标签*****************************
    label_hint_4 =gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_4 ,70,250);
    sungtk_widget_set_font_color(label_hint_4,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_4,30,FALSE);
    //**********************奇数标签*****************************
    label_hint_5=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_5,250,250);
    sungtk_widget_set_font_color(label_hint_5,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_5,30,FALSE);
    //**********************偶数标签*****************************
    label_hint_6=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_6,70,300);
    sungtk_widget_set_font_color(label_hint_6,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_6,30,FALSE);
    //**********************奇数标签*****************************
    label_hint_7=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_7,250,300);
    sungtk_widget_set_font_color(label_hint_7,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_7,30,FALSE);
     //**********************偶数标签*****************************
    label_hint_8=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_8,70,350);
    sungtk_widget_set_font_color(label_hint_8,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_8,30,FALSE);
    //**********************奇数标签*****************************
    label_hint_9=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_9,250,350);
    sungtk_widget_set_font_color(label_hint_9,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_9,30,FALSE);
    //**********************偶数标签*****************************
    label_hint_10=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_10,70,400);
    sungtk_widget_set_font_color(label_hint_10,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_10,30,FALSE);
    //**********************奇数标签*****************************
    label_hint_11=gtk_label_new("");
    gtk_fixed_put(GTK_FIXED(fixed),label_hint_11,250,400);
    sungtk_widget_set_font_color(label_hint_11,"#00FFFF",FALSE);
    sungtk_widget_set_font_size(label_hint_11,30,FALSE); 
}
int main(int argc, char  *argv[])
{
	//******************gtk*********************
    gtk_init(&argc,&argv);//gtk第一句
	WinLogin();//调用生成登录窗口
    WinReg();//调用生成注册窗口
    WinJigsaw();//调用生成拼图窗口
    WinRanking();//调用生成排行榜窗口
    WinPreview();//调用生成预览窗口


    

	gtk_widget_show_all(win_login);//显示登录窗口
    gtk_main(); //循环显示

	return 0;
}