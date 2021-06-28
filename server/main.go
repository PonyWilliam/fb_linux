package main

import (
	"fmt"
	"log"
	"net"
	"os"
	srv "server/srv"
	"strings"
	//"strings"
	//"time"
)
func init(){
	srv.Code = -1
}
func process(conn2 net.Conn){
	defer conn2.Close()
	var f *os.File
	flag := 1
	for{
		if flag == 1{
			_ = os.Remove("1.pcm")
			_, _ = os.Create("1.pcm")
			f,_ = os.OpenFile("1.pcm",os.O_RDWR|os.O_CREATE|os.O_APPEND,0644)
			flag = 0
		}
		buf := make([]byte, 1024)
		n,_ := conn2.Read(buf)
		if n!= 0{
			if strings.Contains(string(buf),"myend666"){
				srv.Listen()
				fmt.Println("已更新")
				//读取所有的内容
				flag = 1
				continue//重新去读取
			}
			_, _ = f.Write(buf[:n])
		}

	}
}
func write(conn2 net.Conn){
	defer conn2.Close()
	fmt.Printf("准备给:%v发送消息\n",conn2.RemoteAddr())
	for{
		if srv.Code == 0 {
			fmt.Println("回传播放上一首")
			_,_ = conn2.Write([]byte("0"))
			srv.Code = -1
		}else if srv.Code == 1 {
			fmt.Println("回传播放下一首")
			_,_ = conn2.Write([]byte("1"))
			srv.Code = -1
		}else if srv.Code == 2 {
			fmt.Println("回传关灯")
			_,_ = conn2.Write([]byte("2"))
			srv.Code = -1
		}else if srv.Code == 3{
			fmt.Println("回传开灯")
			_,_ = conn2.Write([]byte("3"))
			srv.Code = -1
		}else if srv.Code == 4{
			fmt.Println("回传重启")
			_,_ = conn2.Write([]byte("4"))
			srv.Code = -1
		}else if srv.Code == 5{
			fmt.Println("回传暂停")
			_,_ = conn2.Write([]byte("5"))
			srv.Code = -1
		}else if srv.Code == 6{
			fmt.Println("回传播放")
			_,_ = conn2.Write([]byte("6"))
			srv.Code = -1
		}

	}

}
func main(){
	fmt.Println("服务器来了")
	listen,err := net.Listen("tcp","192.168.0.1:6666")
	if err != nil{
		log.Fatal(err)
	}
	defer listen.Close()
	for{
		//循环等待
		conn2,err := listen.Accept()
		if err != nil{
			log.Fatal(err)
		}
		fmt.Println("客户端Ip",conn2.RemoteAddr())
		go write(conn2)
		process(conn2)
	}
}