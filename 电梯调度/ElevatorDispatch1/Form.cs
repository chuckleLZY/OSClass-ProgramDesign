using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ElevatorDispatch1
{
    public partial class Form : System.Windows.Forms.Form
    {
        //数据类型定义
        public const int ELE_NUM = 5;//电梯数量
        public const int FLOOR_NUM = 20;//楼层层数

        public static Dictionary<int, Button> Elevator_inside_button = new Dictionary<int, Button>();//记录电梯内部按钮，ele_num=int/30+1; floor_num=int%30; 开=21; 关=22; 警报=23;
        public static Dictionary<int, Label> Elevator_Label = new Dictionary<int, Label>();//记录电梯到达楼层状态

        public static List<Button> Outside_upbutton = new List<Button>();
        public static List<Button> Outside_downbutton = new List<Button>();

        public static Dictionary<int, Timer> Elevator_Timer = new Dictionary<int, Timer>();//记录电梯Timer
        public static Dictionary<int, Elevator> All_the_Elevator = new Dictionary<int, Elevator>();//记录所有电梯

        public static Dictionary<int, bool> Elevator_press_up_button = new Dictionary<int, bool>();//记录电梯按下上行楼层按钮
        public static Dictionary<int, bool> Elevator_press_down_button = new Dictionary<int, bool>();//记录电梯按下下行楼层按钮

        public List<List<Activity>> Works = new List<List<Activity>>();//进程等待队列
        public List<Activity> work1 = new List<Activity>();
        public List<Activity> work2 = new List<Activity>();
        public List<Activity> work3 = new List<Activity>();
        public List<Activity> work4 = new List<Activity>();
        public List<Activity> work5 = new List<Activity>();

        private int SortByScore(Activity m1,Activity m2)
        {
            return m1.score.CompareTo(m2.score);
        }

        public class Activity
        {
            public int floor = 0;//进程楼层
            public bool updown;//进程向上还是向下，true=up;down=false;
            public int score;
            public bool over;//进程是否完成
        };
        
        public enum Direction { stop, up, down };//电梯运动的方向
        public enum Status { run, open, close };//电梯门的开合状态

        //电梯类（PCB）
        public class Elevator
        {
            public int number;//电梯编号

            //电梯目前状态
            public Direction current_direction = Direction.stop;
            public Status current_status = Status.close;
            public int current_floor = 1;//电梯所处楼层
            public Dictionary<int, bool> Elevator_inside_order = new Dictionary<int, bool>();//记录电梯内部按钮按下情况

            public Elevator(int num)
            {
                for (int i = 1; i <= FLOOR_NUM + 3; i++)
                {
                    Elevator_inside_order.Add(i, false);
                }
                number = num;
            }

        }

        //初始化函数
        public Form()
        {
            InitializeComponent();

            comboBox1.SelectedIndex = 0;

            Works.Add(work1);
            Works.Add(work2);
            Works.Add(work3);
            Works.Add(work4);
            Works.Add(work5);

            //5个电梯
            for (int i=1;i<=ELE_NUM;i++)
            {
                All_the_Elevator.Add(i, new Elevator(i));
            }

            //各个楼层上下按钮状态
            for(int i=1;i<=FLOOR_NUM;i++)
            {
                Elevator_press_up_button.Add(i, false);
                Elevator_press_down_button.Add(i, false);
            }

            //时钟
            int temp = 0;
            foreach (Timer timer in this.components.Components.OfType<Timer>())
            {
                int.TryParse(timer.Tag.ToString(), out temp);
                if(temp != 0)               
                    Elevator_Timer.Add(temp, timer);
            }

            //电梯标签
            temp = 0;
            foreach (Label label in tableLayoutPanel6.Controls.OfType<Label>())
            {
                int.TryParse(label.Tag.ToString(), out temp);
                if (temp > 0)
                    Elevator_Label.Add(temp, label);
            }

            //电梯外部上下按钮
            temp = -1;
            foreach (Button button in tableLayoutPanel6.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if (temp == 0)
                    Outside_upbutton.Add(button);
                else
                    Outside_downbutton.Add(button);
            }

            //电梯内部按钮
            //1号电梯
            temp = -1;
            foreach(Button button in tableLayoutPanel1.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if(temp==1)
                {
                    int temp_floor = 0;
                    int.TryParse(button.Text.ToString(), out temp_floor);
                    Elevator_inside_button.Add((temp - 1) * 30 + temp_floor, button);
                }
                else
                {
                    int special = temp % 10;
                    Elevator_inside_button.Add(30 - 10  +special, button);
                }
            }

            //2号电梯
            temp = -1;
            foreach (Button button in tableLayoutPanel2.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if (temp == 2)
                {
                    int temp_floor = 0;
                    int.TryParse(button.Text.ToString(), out temp_floor);
                    Elevator_inside_button.Add((temp - 1) * 30 + temp_floor, button);
                }
                else
                {
                    int special = temp % 10;
                    Elevator_inside_button.Add(60 - 10 + special, button);
                }
            }

            //3号电梯
            temp = -1;
            foreach (Button button in tableLayoutPanel3.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if (temp == 3)
                {
                    int temp_floor = 0;
                    int.TryParse(button.Text.ToString(), out temp_floor);
                    Elevator_inside_button.Add((temp - 1) * 30 + temp_floor, button);
                }
                else
                {
                    int special = temp % 10;
                    Elevator_inside_button.Add(90 - 10 + special, button);
                }
            }

            //4号电梯
            temp = -1;
            foreach (Button button in tableLayoutPanel4.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if (temp == 4)
                {
                    int temp_floor = 0;
                    int.TryParse(button.Text.ToString(), out temp_floor);
                    Elevator_inside_button.Add((temp - 1) * 30 + temp_floor, button);
                }
                else
                {
                    int special = temp % 10;
                    Elevator_inside_button.Add(120 - 10 + special, button);
                }
            }

            //5号电梯
            temp = -1;            
            foreach (Button button in tableLayoutPanel5.Controls.OfType<Button>())
            {
                int.TryParse(button.Tag.ToString(), out temp);
                if (temp == 5)
                {
                    int temp_floor = 0;
                    int.TryParse(button.Text.ToString(), out temp_floor);
                    Elevator_inside_button.Add((temp - 1) * 30 + temp_floor, button);
                }
                else
                {
                    int special = temp % 10;
                    Elevator_inside_button.Add(150 - 10 + special, button);
                }
            }

        }

        //更新Button、Label数据
        //更新电梯内部按钮
        public void renew_inside_button(int num)
        {
            if (!Elevator_inside_button.ContainsKey(num))
                return;

            //按钮状态为可按
            Elevator elevator = All_the_Elevator[num];
            for(int i= (num - 1) * 30+1;i< (num - 1) * 30 + 20;i++)
                Elevator_inside_button[(num - 1) * 30 + elevator.current_floor].Enabled = !elevator.Elevator_inside_order[elevator.current_floor];

        }

        //更新电梯外部按钮
        public void renew_outside_button()
        {
            ComboBox combobox = this.Controls.Find("comboBox1", true)[0] as ComboBox;
            int current_floor = 0;
            int.TryParse(combobox.Text, out current_floor);
            bool upEable = true, downEable = true;

            if (!Elevator_press_up_button.ContainsKey(current_floor))
                return;

            upEable = !Elevator_press_up_button[current_floor];
            downEable = !Elevator_press_down_button[current_floor];

            //1楼下行按钮和20楼上行按钮无效
            if (current_floor == 1)
                downEable = false;
            if (current_floor == FLOOR_NUM)
                upEable = false;

            foreach (Button button in Outside_upbutton)
                button.Enabled = upEable;
            foreach (Button button in Outside_downbutton)
                button.Enabled = downEable;
        }

        //更新电梯运行标签Label
        public void renew_ele_label(int num)
        {
            Elevator elevator = All_the_Elevator[num];
            string txt = elevator.current_floor.ToString();
            
            if (elevator.current_direction == Direction.up)
                txt += "↑";
            if (elevator.current_direction == Direction.down)
                txt += "↓";
            if (elevator.current_direction == Direction.stop)
            {
                if (elevator.current_status == Status.open)
                {
                    txt += "(open)";
                    Elevator_inside_button[num * 30 - 9].Enabled = false;
                    Elevator_inside_button[num * 30 - 8].Enabled = true;
                }
                if (elevator.current_status == Status.close)
                {
                    txt += "(close)";
                    Elevator_inside_button[num * 30 - 9].Enabled = true;
                    Elevator_inside_button[num * 30 - 8].Enabled = false;
                }
            }

            Elevator_Label[num].Text = txt;    
        }

        //响应函数
        //电梯内部按钮
        private void inside_Click(object sender, EventArgs e)
        {
            Button button = sender as Button;
            button.Enabled = false;//电梯按钮无法重复点击

            int current_num = 0;//确定电梯编号
            int current_floor = 0;//确定层数
            int.TryParse(button.Tag.ToString(), out current_num);
            int.TryParse(button.Text, out current_floor);

            //创建进程到等待队列
            inside_dispatch(current_num,current_floor);
        }

        //开，关，警报按钮
        private void special_Click(object sender, EventArgs e)
        {
            Button button = sender as Button;
            int current_num = 0;//确定电梯编号
            int current_order = 0;//确定操作数，1为开，2为关，3为警报
            int.TryParse(button.Tag.ToString(), out current_num);
            current_order=current_num%10;
            current_num /= 10;
            Elevator elevator = All_the_Elevator[current_num];
            switch (current_order)
            {
                case 1:
                    if(elevator.current_direction==Direction.stop)
                    {
                        if(elevator.current_status == Status.close)
                        {
                            button.Enabled = false;
                            Elevator_inside_button[current_num * 30 -8].Enabled = true;
                            All_the_Elevator[current_num].current_status = Status.open;
                            renew_ele_label(current_num);
                        }
                        else
                        {
                            MessageBox.Show("电梯已经打开。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                    }
                    else
                    {
                        MessageBox.Show("电梯正在运行，无法开门！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    break;
                case 2:
                    if (elevator.current_direction == Direction.stop)
                    {
                        if (elevator.current_status == Status.open)
                        {
                            button.Enabled = false;
                            Elevator_inside_button[(current_num - 1) * 30 + 21].Enabled = true;
                            All_the_Elevator[current_num].current_status = Status.close;
                            renew_ele_label(current_num);
                        }
                        else
                        {
                            MessageBox.Show("电梯已经关闭。", "提示", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                    }
                    else
                    {
                        MessageBox.Show("电梯正在运行，无法操作！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    }
                    break;
                case 3:
                    MessageBox.Show(current_num.ToString()+"号电梯已报警！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    break;
                default:break;
            }

        }

        //电梯外部按钮      
        private void updown_Click(object sender, EventArgs e)
        {
            Button button = sender as Button;
            ComboBox combobox = this.Controls.Find("comboBox1", true)[0] as ComboBox;
            int floor = 0;//楼层
            int.TryParse(combobox.Text, out floor);
            if(button.Text.Contains("↑"))
            {
                Elevator_press_up_button[floor] = true;
                renew_outside_button();
                outside_dispatch(floor,true);
            }
            else
            {
                Elevator_press_down_button[floor] = true;
                renew_outside_button();
                outside_dispatch(floor,false);
            }

        }


        //切换楼层
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            renew_outside_button();
        }

        //Timer时刻更新
        //电梯楼层Label更新
        private void Elevator_Tick(object sender, EventArgs e)
        {
            Timer timer = sender as Timer;
            int current_num = 0;
            int.TryParse(timer.Tag.ToString(), out current_num);
            Elevator elevator = All_the_Elevator[current_num];


            if (Works[current_num - 1].Count == 0)//无进程
                All_the_Elevator[current_num].current_direction = Direction.stop;
            else
            {
                Activity activity = Works[current_num - 1][0];
                int distance = elevator.current_floor - activity.floor;
                if (distance > 0)
                {
                    All_the_Elevator[current_num].current_direction = Direction.down;
                    All_the_Elevator[current_num].current_status = Status.run;
                }
                else if (distance < 0)
                {
                    All_the_Elevator[current_num].current_direction = Direction.up;
                    All_the_Elevator[current_num].current_status = Status.run;
                }
                else if (distance == 0)
                {
                    All_the_Elevator[current_num].current_status = Status.open;
                    All_the_Elevator[current_num].current_direction = Direction.stop;
                    Works[current_num - 1][0].over = true;
                    if(Works[current_num-1][0].updown)                    
                        Elevator_press_up_button[activity.floor] = false;                   
                    else
                        Elevator_press_down_button[activity.floor] = false;
                }
            }

            //电梯楼层状态更新
            if (elevator.current_direction != Direction.stop)
            {
                if (elevator.current_direction == Direction.up)
                    elevator.current_floor++;
                else if (elevator.current_direction == Direction.down)
                    elevator.current_floor--;

                if (elevator.Elevator_inside_order[elevator.current_floor])
                {
                    elevator.Elevator_inside_order[elevator.current_floor] = false;
                    elevator.current_direction = Direction.stop;
                    elevator.current_status = Status.open;
                }                
            }
            else
            {
                elevator.current_status = Status.close;
            }
            renew_ele_label(current_num);
            renew_inside_button(current_num);
            renew_outside_button();
        }

        //电梯调度的进程更新
        private void ElevatorDispatch_Tick(object sender, EventArgs e)
        {
            for (int i=1;i<=ELE_NUM;i++)
            {               
                //优先级调度
                for (int j = 0; j < Works[i - 1].Count; j++)
                {
                    if (Works[i - 1][j].over == true)
                    {
                        All_the_Elevator[i].Elevator_inside_order[Works[i - 1][j].floor] = false;
                        Elevator_press_up_button[Works[i - 1][j].floor] = false;
                        Elevator_press_down_button[Works[i - 1][j].floor] = false;
                        Works[i - 1].RemoveAt(j);
                        j--;
                    }
                    else
                    {
                        if (j == 0)
                        {
                            Works[i - 1][j].score = 0;
                            continue;
                        }
                        bool now_updown = Works[i - 1][0].updown;
                        int destination = Works[i - 1][0].floor;
                        int now_floor = All_the_Elevator[i].current_floor;
                        int dir = 1;
                        if (!now_updown)//下
                            dir = -1;

                        if (Works[i - 1][j].updown == now_updown && dir * now_floor <= dir * Works[i - 1][j].floor)
                            Works[i - 1][j].score = Math.Abs(Works[i - 1][j].floor - now_floor);
                        else Works[i - 1][j].score = Math.Abs(2 * destination - now_floor - Works[i - 1][j].floor);
                    }
                    
                }
                Works[i - 1].Sort(SortByScore);
            }
        }


        //调度算法
        //电梯内部调度
        public  void inside_dispatch(int num,int floor)
        {
            //电梯更新
            Elevator elevator = All_the_Elevator[num];
            elevator.Elevator_inside_order[floor] = true;
            int distance = elevator.current_floor - floor;
            Activity activity = new Activity();
            activity.floor = floor;
            activity.over = false;

            if (distance > 0)
                activity.updown = false;
            else if (distance < 0)
                activity.updown = true;
            else if (distance == 0)
                activity.over = true;

            Works[num - 1].Add(activity);
        
        }

        //电梯外部调度
        public  void outside_dispatch(int floor,bool updown)
        {
            Activity activity = new Activity();
            activity.floor = floor;
            activity.over = false;
            activity.updown = updown;
            int current_ele = 1;
            int min = 100;
            //计算优先值，最小的优先
            for(int i=1;i<=ELE_NUM;i++)
            {
                if (Works[i - 1].Count == 0)
                {
                    int ans = Math.Abs(floor - All_the_Elevator[i].current_floor);
                    if (ans < min)
                    {
                        min = ans;
                        current_ele = i;
                    }
                    continue;
                }
                bool now_updown = Works[i - 1][0].updown;
                int now_order = Works[i - 1][0].floor;
                int now_floor = All_the_Elevator[i].current_floor;
                if (activity.updown==now_updown)
                {
                    int dir = 1;
                    if (!activity.updown)//下
                        dir = -1;

                    if (dir*now_floor > dir*activity.floor)
                    {
                        int ans = dir * (2 * now_order - now_floor - activity.floor);
                        if (ans < min)
                        {
                            min = ans;
                            current_ele = i;
                        }
                    }
                    else if (dir*now_floor <= dir*activity.floor)
                    {
                        int ans = dir * (activity.floor - now_floor);
                        if (ans < min)
                        {
                            min = ans;
                            current_ele = i;
                        }
                    }
                }
                else
                {
                    int ans =Math.Abs(2 * now_order - now_floor - activity.floor);
                    if (ans < min)
                    {
                        min = ans;
                        current_ele = i;
                    }
                }
            }
            inside_dispatch(current_ele, floor);
        }
    }
}
