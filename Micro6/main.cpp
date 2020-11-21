//
//  main.cpp
//  Micro1_2
//
//  Created by Исыпов Илья on 12.11.2020.
//

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <deque>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <csignal>
#include <sys/types.h>      /* needed to use pid_t, etc. */
#include <sys/wait.h>       /* needed to use wait() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         /* LINUX constants and functions (fork(), etc.) */
#include <fcntl.h>
using namespace std;
int ended = 1;
vector< vector<char *> > argv1;


void ProcessTime() {

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    
    timeval user_tv, sys_tv;
    user_tv = usage.ru_utime;
    sys_tv = usage.ru_stime;
    
    cerr << "user " << user_tv.tv_sec <<"."<<user_tv.tv_usec<<" sec"<<endl;
    cerr << "system " << sys_tv.tv_sec <<"."<<sys_tv.tv_usec<<" sec"<<endl;
}


void PipConv(vector< vector<char *> > & argv, int n) {
    //conveer
    for (int i = 0; i < n - 1; i++) {
        int fd[2];
    pipe(fd);
    pid_t pid = fork();
        if (pid == 0) {
            dup2(fd[1], 1);
            close(fd[0]);
            execvp(argv[i][0], &argv[i][0]);
            exit(0);
        } else {
            close(fd[1]);
            dup2(fd[0], 0);
        }
    }
    //ended = 1;
    //close(1);
    //string b = "b";
    //const char *name1 = b.c_str();
    //open(name1, O_RDWR | O_CREAT |O_TRUNC, 0666);
    cout << "ok";
    execvp(argv[n-1][0], &argv[n-1][0]);
    /*
    pid_t t = fork();
    if (t == 0) {
        execvp(argv[n-1][0], &argv[n-1][0]);
        exit(0);
    } else {
        int info;
        waitpid(t, &info, 0);
    }
     */
    
    
    //stdin.clear();
    //ended = 1;
    exit(0);
}


string CurDir () {
    char dir[FILENAME_MAX];
    getcwd(dir, FILENAME_MAX);
    //for (int i = 0; i < 10; i++) cout << dir[i];
    string CD = string(dir);
    return CD;
}

void ExecPwd1() {
    //close(1);//descriptor вывода
    //const char *name1 = a.c_str();
    //int t = open(name1, O_RDWR | O_CREAT |O_TRUNC, 0666);
    
    
    cout << CurDir() << "\n";
   
    //close(t);
}

int ExecCd(vector<string> &command) {
    //cout <<"cd str"<<command[1];
    cout <<"CD\n";
    if(command.size() != 2) {
        cout << "cd size error"<<endl;
        return 0;
    }
    else {
        if (chdir(command[1].c_str()) != 0) {
            cout << "cd exec error";
        }
        return 1;
    }
    
}


int Acception(char *word, char *rv) {
    if(*word == 0) return *rv == 0 | *rv == '/'| (*rv == '*' && strlen(rv) == 1 );
    switch(*rv) {
        case '*':{
            char *w = word;
            do {
                if(Acception(w, rv+1))
                    return 1;
            } while(*w++);
    
            return 0;
        }
        case '?':{
            return Acception(word+1, rv+1);
        }
        default: {
            return *word == *rv && Acception(word+1, rv+1);
        }
    }
}

void ls(string const &name, bool recursive, vector<string> &ret) {
  //printf("ls(%s)\n", name.c_str());
  DIR *dir = opendir(name.c_str());
  if (dir == nullptr) return;
  for (dirent *d = readdir(dir); d != nullptr; d = readdir(dir)) {
    //printf("ls(%s): %s\n", name.c_str(), d->d_name);
    if (recursive && d->d_type == DT_DIR) {
      if (strcmp(d->d_name, ".") == 0) continue;
      if (strcmp(d->d_name,"..") == 0) continue;
        //cout <<"ok "<<d->d_name<<endl;
        ret.push_back(d->d_name);
      //ls(name + string("/") + d->d_name, true, ret);
    } else {
      ret.push_back(d->d_name);
    }
  }
  closedir(dir);
}

void lsdir (string const &name, bool recursive, vector<string> &ret) {
    //printf("ls(%s)\n", name.c_str());
    DIR *dir = opendir(name.c_str());
    if (dir == nullptr) return;
    for (dirent *d = readdir(dir); d != nullptr; d = readdir(dir)) {
      //printf("ls(%s): %s\n", name.c_str(), d->d_name);
        
      if (recursive && d->d_type == DT_DIR) {
        if (strcmp(d->d_name, ".") == 0) continue;
        if (strcmp(d->d_name,"..") == 0) continue;
          //cout <<"ok dir "<<d->d_name<<endl;
          ret.push_back(d->d_name);
       // ls(name + string("/") + d->d_name, true, ret);
      }
        //ret.push_back(d->d_name);
         
    }
    closedir(dir);
  }

int StarSubsDir(string word1, vector<string> &variants) {
    
    string dir = CurDir();
    //cout <<"DIR: "<<dir<<endl;
    vector<string> files;
    lsdir(dir, true, files);
    for (auto file: files) {
        
        //char *word = word1.c_str();
        char *rv = new char[word1.length() + 1];
        strcpy(rv, word1.c_str());
        
        char *word = new char[file.length() + 1];
        strcpy(word, file.c_str());
        //Acception(word, rv);
        if(Acception(word, rv) == 1) {
            //cout <<"file: "<<file<<endl;
            //cout << "oki " << file<<endl;
            variants.push_back(file);
        }
    }
    //cout <<"END for "<<word1<<endl;
    
    return 1;
}

int StarSubs(string word1, vector<string> &variants) {
    string dir = CurDir();
    vector<string> files;
    ls(dir, true, files);
    for (auto file: files) {
        
        //char *word = word1.c_str();
        char *rv = new char[word1.length() + 1];
        strcpy(rv, word1.c_str());
        
        char *word = new char[file.length() + 1];
        strcpy(word, file.c_str());
        //Acception(word, rv);
        if(Acception(word, rv) == 1) {
            //cout << "oki " << file<<endl;
            variants.push_back(file);
        }
    }
    
    return 1;
}

int StarDir(string direction, string start_dir, vector<string> &variants) {
    
    if(direction.size() == 1) {
        chdir("/");
        vector<string> vars;
        StarSubs("*", vars);
        for (vector<string>::iterator it = vars.begin() ; it!=vars.end() ; ++it) {
            variants.push_back("/"+*it);
            //cout <<"VARS: " <<*it <<endl;
        }
        return 1;
    }
    //string start_dir = CurDir();
    vector<string> dirs = {""};
    //разбиваем по '/'
    for(int i = 1; i < direction.size(); i++) {
        if(direction[i] !=  '/') {
            dirs[dirs.size() - 1]+=direction[i];
        }
        else dirs.push_back("");
    }
    string go_dir = "";
    int i;
    for(i = 0; i < dirs.size(); i++) {
        if((dirs[i].find('*') == -1) && (dirs[i].find('?') == -1)) {
            //cout <<"dobav"<<dirs[i]<<endl;
            go_dir+="/"+dirs[i];
        } else {
            break;
        }
        //cout << dirs[i] <<"&"; cout << endl;
    }
    //cout<<"godir: "<<go_dir<<endl;
    if (i == dirs.size()) {
        vector<string> vars;
        //cout << "oop"<< i <<endl;
        //cout <<"dirgo: "<<dirs[i - 1]<<endl;
        if(chdir(go_dir.c_str()) != -1) variants.push_back(go_dir);
        /*
        StarSubsDir(dirs[i - 1], vars);
        if(vars.size()!=0)
            //cout << "pushim:"<<direction<<endl;
            variants.push_back(direction);
         */
        return 0;
    }
    //cout <<"go dir "<<go_dir<<endl;
    string tale = "";
    int j;
    for (j = i+1; j < dirs.size() - 1; j++) {
        if(dirs[j]!="")
            tale+=dirs[j] + "/";
    }
    if(dirs[j]!="")
        tale+=dirs[j];
    //cout << "dir i " << dirs[i] << endl;
    if(chdir(go_dir.c_str()) == -1 && go_dir!="") return 0;
    //cout << "here :" << CurDir();
    vector<string> vars;
    StarSubsDir(dirs[i], vars);
    for (vector<string>::iterator it = vars.begin() ; it!=vars.end() ; ++it) {
        //cout <<"go_dir " << go_dir <<" it: "<<*it<<" tale: " << tale <<endl;
        //cout << "direct "<<direction<<endl;
        //cout <<"now dir "<< CurDir()<<endl;
        //cout <<"it: "<< *it << endl;
        //cout <<"tale: "<< tale << endl;
        //cout << go_dir + "/" + *it + "/" + tale << endl;
        if(tale!="" && tale!="/") {
            //cout <<"kuda: "<< go_dir + "/" + *it + "/" + tale<<endl;
            StarDir(go_dir + "/" + *it + "/" + tale, start_dir, variants);
        }
        else {
            //cout <<"kuda: "<< go_dir + "/" + *it<<endl;
            StarDir(go_dir + "/" + *it, start_dir, variants);
        }
        //else
        //cout << go_dir;
           // cout<<*it <<endl;
    }
    return 0;
}


int StarLine(vector<string> &commands) {
    //подставляем вместо *? в строке
    vector<string> commands1;
    for (int i = 0; i < commands.size(); i++) {
        if(commands[i].find('/') != -1 && (commands[i].find('*') != -1 || commands[i].find('?') != -1)) {
            string start_dir = CurDir();
            vector<string> variants = {};
            chdir("/");
            //cout << "comi: "<<commands[i]<<endl;
            StarDir(commands[i], CurDir(), variants);
            //cout <<"Answer\n";
            for (vector<string>::iterator it = variants.begin() ; it!=variants.end() ; ++it) {
                commands1.push_back(*it);
                //cout << *it<<endl;
            }
            chdir(start_dir.c_str());
            if(variants.size() == 0 && commands[i]!="/") {
                cout << "no coincedence" << endl;
                commands.clear();
                return 0;
            }
        } else
        if(commands[i].find('*') != -1 || commands[i].find('?') != -1) {
            vector<string> variants;
            StarSubs(commands[i], variants);
            for (vector<string>::iterator it = variants.begin() ; it!=variants.end() ; ++it) {
                commands1.push_back(*it);
                   // cout<<*it <<endl;
            }
            if(variants.size() == 0) {
                cout << "no coincedence" << endl;
                commands.clear();
                return 0;
            }
        } else {
            commands1.push_back(commands[i]);
        }
        
    }
    commands = commands1;
    return 0;
}

int ExecCom(vector<string> command, string a, string b, int k){
    vector<char *> arg;
    for (int i = 0; i < command.size(); ++i) {
        //cout << "comi:"<<command[i].c_str()<<endl;
        arg.push_back((char *) command[i].c_str());
    }
    arg.push_back(NULL);
    
    
    //int fd[2];
    //pipe(fd);
        pid_t pid = fork();
        if (pid == 0) {
            //cout << "ok3";
            if(k!=0) {
                close(0);//descriptor ввода
                const char *name = a.c_str();
                open(name, O_RDWR | O_CREAT, 0666);
            }
            close(1);//descriptor вывода
            const char *name1 = b.c_str();
            open(name1, O_RDWR | O_CREAT |O_TRUNC, 0666);
            //cout << "ok4"<<arg[0]<<endl;
            execvp(arg[0], &arg[0]);
            //cout << "ok6";
        } else {
            int info;
            //string line;
            //close(0);
            //const char *name = a.c_str();
            //int t = open(name, O_RDWR | O_CREAT, 0666);
            //dup2(t, 0);
            
            waitpid(pid, &info, 0);
            //close(t);
            //dup2(0,0);
        }
    return 0;
}

void Out(vector<string> commands, string &filename, int sign, string &inoutfile, int k) {
    //для вида com <(>) a sign: 1 == >
    string cod = commands [0];
    vector<char *> args;
    for (int i = 0; i < commands.size()-2; ++i) {
        args.push_back((char *) commands[i].c_str());
    }
    args.push_back(NULL);
    
    if (sign) {
        pid_t pid = fork();
        if (pid == 0) {
            if(k != 0) {
                close(0);//descriptor ввода
                const char *name1 = inoutfile.c_str();
                open(name1, O_RDWR | O_CREAT, 0666);
            }
            close(1);//descriptor вывода
            const char *name = filename.c_str();
            open(name, O_RDWR | O_CREAT | O_TRUNC, 0666);
            //делаем а файлом вывода
            execvp(cod.c_str(), &args[0]);
        } else {
            int info;
            waitpid(pid, &info, 0);
        }
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            close(0);
            const char *name = filename.c_str();
            open(name, O_RDWR | O_CREAT, 0666);
            
            close(1);
            const char *name1 = inoutfile.c_str();
            open(name1, O_RDWR | O_CREAT | O_TRUNC, 0666);
            //делаем а файлом ввода.
            execvp(cod.c_str(), &args[0]);
        } else {
            int info;
            //cout << info;
            waitpid(pid, &info, 0);
        }
    }
}

void Out2(vector<string> commands, string &filename1, string &filename2) {
    string cod = commands [0];
    vector<char *> args;
    for (int i = 0; i < commands.size()-4; ++i) {
        args.push_back((char *) commands[i].c_str());
    }
    args.push_back(NULL);
    
    
    //cases com < a > b & com > a < b
    int fd[2];
    pipe(fd);
    // fn1 > com > fn2
        pid_t pid = fork();
        if (pid == 0) {
            close(0);//descriptor ввода
            const char *name = filename1.c_str();
            open(name, O_RDWR | O_CREAT, 0666);
            
            close(1);//descriptor вывода
            const char *name1 = filename2.c_str();
            open(name1, O_RDWR | O_CREAT | O_TRUNC, 0666);
            execvp(cod.c_str(), &args[0]);
        } else {
            int info;
            waitpid(pid, &info, 0);
            
        }
    
}

int LineExec1(deque<vector<string>> &commands) {
    int qnt = commands.size();
    
    string fout = "";
    string fin = "";
    
    int proc_timer = 0, k = 0;
    long start_t, end_t;
    start_t = clock();
    for(int w = 0; w < qnt; w++) {
        int p = 0;
        vector<string> command = commands[w];
        long len = command.size();
        
        for (long i = 0; i < len; i++) {
            //cout <<"com i:" << command[i]<<endl;
            if(command[i].find('*')!=-1 || command[i].find('?')!=-1 || command[i].find('/')!=-1) {
                //удаляем *?
                StarLine(commands[w]);
                //cout <<"com:\n";
                //for(int z = 0; z < commands[w].size(); z++) cout <<commands[w][z]<< endl;
                break;
            }
        }
        
        for (long i = 0; i < len; i++) {
            if(command[i] == "<" || command[i] == ">") {
                //cout << "comi:" << command[i]<<endl;
                p++;
            }
        }
        if(command[0] == "time") {
            cout << "time)\n";
            proc_timer = 1;
            
            commands[w].erase(commands[w].begin());
        }
        
        if(p == 2) {
            //cout << "ok";
            if(command.size() < 5) return 0;
            if(command[command.size() - 4] == "<" && command[command.size() - 2] == ">") {
                if(commands.size() == 1 && k == 0) {
                    Out2(command, command[command.size() - 3], command[command.size() - 1]);
                    return 1;
                } else {
                    cout << "< & > in conv\n";
                    return 0;
                }
            } else if(command[command.size() - 4] == ">" && command[command.size() - 2] == "<") {
                if(commands.size() == 1 && k == 0) {
                    Out2(command, command[command.size() - 1], command[command.size() - 3]);
                    return 1;
                } else {
                    cout << "< & > in conv\n";
                    return 0;
                }
            } else {
                cout << "< & > format error\n";
                return 0;
            }
        } else
            if(p == 1) {
                if(command.size() < 3) return 0;
                if(command[command.size() - 2] == "<") {
                    //cout <<"Gagag\n";
                    if(k!=0) {
                        cout <<"com < a not in start of conv\n";
                        return 0;
                    } else {
                        //cout << "ok";
                        fin = command[command.size() - 1];
                        //cout <<"to erase: "<< commands[w][command.size()-1]<<endl;
                        //cout <<"to erase: "<< commands[w][command.size()-2]<<endl;
                        commands[w].pop_back();
                        commands[w].pop_back();
                        //commands[w].
                        //command[w].pop_back();
                        //cout << "end:"<< command[command.end()];
                        //command.erase(command.end() - 1);
                        //command[w].erase(command.end());
                        //Out(command, command[command.size() - 1], 0, b, k);
                        //return 1;
                    }
                //if(k == 0)
                }
                else if (command[command.size() - 2] == ">") {
                    if(commands.size() != w + 1) {
                        cout <<"com > a not in end of conv\n";
                        //cout << commands.size()<<" "<<k;
                        return 0;
                    } else {
                        fout = command[command.size() - 1];
                        commands[w].pop_back();
                        commands[w].pop_back();
                        //cout << "ok >\n";
                        //if(k%2 == 1) {
                            //Out(command, command[command.size() - 1], 1, b, k);
                        //} else {
                            //Out(command, command[command.size() - 1], 1, a, k);
                        //}
                        //чистим вывод
                        //ofstream fout1;
                        //fout1.open(a);
                        //fout1.close();
                        //fout1.open(b);
                        //fout1.close();
                        //return 1;
                    }
                } else {
                    cout <<"Error in <(>) format\n";
                    return 0;
                }
        
    }  else if(command[0] == "cd") {
        if(k!=0 || commands.size() > 1) {
            cout << "cd in conv error\n";
            return 0;
        }
        ExecCd(commands[w]);
    }
    else if (command[0] == "pwd") {
        //cout << "pwd case\n";
        if(k!=0) {
            cout << "pwd not in conv start\n";
            return 0;
        }
        ExecPwd1();
        return 0;
    }
        k++;
    }
    
    //cout << commands.size()<<endl;
    
    vector< vector<char *> > argv;
    for(int i = 0; i < qnt; i++) {
        
        vector<char *> arg;
        for (int j = 0; j < commands[i].size(); j++) {
            //cout << "comi:"<<commands[i][j].c_str()<<endl;
            arg.push_back((char *) commands[i][j].c_str());
        }
        arg.push_back(NULL);
        
        argv.push_back(arg);
    }
    //cout <<"ravn:"<<(argv == argv1);
    //argv1 = argv;
    
    for(int i = 0; i < qnt; i++) {
        for(int j = 0; j < argv[i].size() ; j++) {
           //cout <<"ravn:"<<i<<" "<<j<<" "<<(argv[i][j] == argv1[i][j])<<endl;
            //cout << argv[i][j]<<endl;
            //argv1 = argv;
            //cout <<"look:"<< argv[i][j] << endl;
        }
    }
   
    //cout << "fin: "<< fin<<endl;
    //cout <<"checking1\n";
    pid_t pid = fork();
            if (pid == 0) {
            //signal(2, SIG_DFL);
                if (! fout.empty()) {
                    close(1);
                    dup2(open(fout.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666), 1);
                }
                if (! fin.empty()) {
                    close(0);
                    dup2(open(fin.c_str(), O_RDWR|O_CREAT, 0666), 0);
                }
            PipConv(argv, qnt);
            } else {
            int status;
            //wait(&status);
            waitpid(pid, &status, 0);
                
            }
    if(proc_timer == 1) {
        end_t =clock();
        cerr <<"walltime "<<(double) (end_t - start_t) / 1000 <<" sec"<<endl;
        ProcessTime();
    }
    //cout <<"checking2\n";
    //EPIP(argv, qnt);
    /*
    for(int i = 0; i < qnt; i++) {
        for(int j = 0; j < argv[i].size() - 1; j++) {
            cout << argv[i][j] << endl;
        }
    }
    */
    
    
    return 1;
}


int LineProcessing(string line){
    //разбиваем каждую команду на string элементы
    //cout <<"line: "<<line<<endl;
    deque<vector<string>> commands;
    commands.push_back(vector<string>());
    //long len = line.length();
    string com = "";
    
    char *word;
    char *cstr = new char[line.length() + 1];
    strcpy(cstr, line.c_str());
    word = strtok(cstr, " \t\r\n\a");
    while (word != NULL)
    {
        string s = word;
        if (s == "|"){
            //cout << "\n";
            commands.push_back(vector<string>());
            //commands содержат компоненты конвеера
        }
        else {
            //cout << s;
            commands.back().push_back(s);
        }
        word = strtok(NULL, " \t\r\n\a");
    }
    //cout <<"oop\n";
    //CommandsExec(commands);
    LineExec1(commands);
    //cout <<"checking3\n";
    return 1;
    /*
    int t = commands.size();
    cout << "com qnt:" << t <<"\n";
    for (int i = 0; i < t; i++) {
        for(int j = 0; j < commands[i].size(); j++) cout << commands[i][j] << " ";
        cout << endl;
    }
     */
}

int start1(){
    string UserName = getenv("USER");
    
    

    chdir("/Users/isypov/Desktop/test");
    //

    while(! feof(stdin)){
        cout <<CurDir();
        if (UserName == "root") {
            cout << "!";
        } else {
            cout << ">";
        }
        string line;
        //getline(cin, line);
        while(line.length() == 0) {
            getline(cin, line);
        }
        LineProcessing(line);
        }
        
    return 1;
   
}

int start(){
    string UserName = getenv("USER");
    string pn;
    string line;
    if (UserName == "root") {
        pn = "!";
    } else {
        pn = ">";
    }
    //chdir("/Users/isypov/Desktop/test");
    //
    while(! feof(stdin)){
        
        cout <<CurDir() << pn;
        
        if(getline(cin, line)){
        if(line.length()!= 0 ) {
            //cout << "look:" << line << endl;
            LineProcessing(line);
            //getline(cin, line);
        
        
            //else return 1;
        }
        else break;
            //else
        }
        //else return 1;
        //cout << "work\n";
    }
    return 1;
   
}

int proc;
void signal_handler(int signal) {
    cout << "hell\n";
    if (proc) {
        cout << endl;
        cout << "home"<<endl;
    }
}


int main(int argc, const char * argv[]) {
    proc = 1;
    chdir("/Users/isypov/Desktop/test");
    start();
    
    //deque<vector<string> > commands = {{"cat", "<", "a"},{"cat1"},{"cat2", ">", "b"}};
    //deque<vector<string> > commands = {{"ls", "-l", "a"},{"cat"},{"cat"},{"cat"},{"cat"}};
    //deque<vector<string> > commands = {{"ls", "-lR", "/"},{"more"}};
    //deque<vector<string> > commands = {{"cat", "<", "a"},{"cat"},{"cat", ">", "b"}};
    //deque<vector<string> > commands = {{"ls", "-l", "b", "a"}};
    //cout << commands[1].size();
    //LineExec1(commands);
    //cout <<"hello";
    return 0;
}



