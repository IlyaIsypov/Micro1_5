

#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <deque>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;


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
    
    execvp(argv[n-1][0], &argv[n-1][0]);
    exit(0);
}


string CurDir () {
    char dir[FILENAME_MAX];
    getcwd(dir, FILENAME_MAX);
    string CD = string(dir);
    return CD;
}

void ExecPwd1() {
    
    cout << CurDir() << "\n";
   
    
}

int ExecCd(vector<string> &command) {

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
    
    vector<string> files;
    lsdir(dir, true, files);
    for (auto file: files) {
        
        //char *word = word1.c_str();
        char *rv = new char[word1.length() + 1];
        strcpy(rv, word1.c_str());
        
        char *word = new char[file.length() + 1];
        strcpy(word, file.c_str());
        
        if(Acception(word, rv) == 1) {
            variants.push_back(file);
        }
    }
    
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
        
        if(Acception(word, rv) == 1) {
          
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
        }
        return 1;
    }
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
            go_dir+="/"+dirs[i];
        } else {
            break;
        }
        
    }
    //cout <<"ok1\n";
    if (i == dirs.size()) {
        vector<string> vars;
        if(chdir(go_dir.c_str()) != -1) variants.push_back(go_dir);
        return 0;
    }
    //cout <<"go dir "<<go_dir<<endl;
    string tale = "";
    int j;
    for (j = i+1; j < dirs.size() - 1; j++) {
        //cout <<"dirs j"<<dirs[j]<<endl;
        cout <<"ok?/n";
        if(dirs[j]!="")
            tale+=dirs[j] + "/";
    }
    if (j == dirs.size() - 1){
        if(dirs[j]!="")
            tale+=dirs[j];
    }
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
            cout <<"kuda: "<< go_dir + "/" + *it + "/" + tale<<endl;
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
            StarDir(commands[i], CurDir(), variants);
            chdir(start_dir.c_str());
            for (vector<string>::iterator it = variants.begin() ; it!=variants.end() ; ++it) {
                cout <<"vars: "<< *it << endl;
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
                close(0);
                const char *name1 = inoutfile.c_str();
                open(name1, O_RDWR | O_CREAT, 0666);
            }
            close(1);
            const char *name = filename.c_str();
            open(name, O_RDWR | O_CREAT | O_TRUNC, 0666);
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
            execvp(cod.c_str(), &args[0]);
        } else {
            int info;
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
            close(0);
            const char *name = filename1.c_str();
            open(name, O_RDWR | O_CREAT, 0666);
            
            close(1);
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
                StarLine(commands[w]);
                
                break;
            }
        }
        
        for (long i = 0; i < len; i++) {
            if(command[i] == "<" || command[i] == ">") {
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
                        commands[w].pop_back();
                        commands[w].pop_back();
                        
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
   
    vector< vector<char *> > argv;
    for(int i = 0; i < qnt; i++) {
        
        vector<char *> arg;
        for (int j = 0; j < commands[i].size(); j++) {
            arg.push_back((char *) commands[i][j].c_str());
        }
        arg.push_back(NULL);
        
        argv.push_back(arg);
    }
    
    
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
            
            waitpid(pid, &status, 0);
                
            }
    if(proc_timer == 1) {
        end_t =clock();
        cerr <<"walltime "<<(double) (end_t - start_t) / 1000 <<" sec"<<endl;
        ProcessTime();
    }
    
    return 1;
}


int LineProcessing(string line){
    //разбиваем каждую команду на string элементы
    
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

            commands.push_back(vector<string>());
        }
        else {
     
            commands.back().push_back(s);
        }
        word = strtok(NULL, " \t\r\n\a");
    }
    
    LineExec1(commands);
    
    return 1;
}


int Microsha(){
    string UserName = getenv("USER");
    string pn;
    string line;
    //SigCreate();
    
    if (UserName == "root") {
        pn = "!";
    } else {
        pn = ">";
    }
    while(! feof(stdin)){
        
        cout <<CurDir() << pn;
        
        if(getline(cin, line)){
        if(line.length()!= 0 ) {
            //raise( SIGINT);
            LineProcessing(line);
            
        }
        else break;
        }
    
    }
    return 1;
   
}


int main(int argc, const char * argv[]) {
    chdir("/Users/isypov/Desktop/test");
    
    //signal(SIGINT, SIG_IGN);
    Microsha();
    
    return 0;
}



