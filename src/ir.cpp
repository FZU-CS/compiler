#include <cstdio>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <map>
#include <set>
using namespace std;

int GRAMMAR_CNT = 25;
int TERMINA_CNT = 25;
int NONTERMINA_CNT = 11;
int ClosureId = 0;
vector<int> RIGHT[15];

struct Token {
    int type; // 1非终结符 2终结符
    string val;
}nonTermina[15],termina[30];

struct Grammar {
    int id;
    vector<Token> left,right;
    string leftString,rightString;
    bool operator < (const Grammar &g) const {
        if(leftString == g.leftString) return rightString < g.rightString;
        return leftString < g.leftString;
    }
} gs[30];
struct F {
    set<string> v;
} firsts[60],follows[60];

struct Closure {
    int id;
    vector<pair<Grammar,int> > clo;
    vector<pair<int,int> > record;
    map<string,int> succeed_s;//移入
    map<string,int> succeed_r;//规约
} closures[100],cloTmp[100];

map<string,int> unit;
map<pair<int,string> ,int> gotoMap;
string s[15]={"S'","S","compound_stmt","stmts","stmt","bool","expr","factor","optional_stmts","id_lists","term"};
string s2[30]={"program","id",",",";",".","begin","end",":=","while",
"do","if","then","else","repeat","until","<","+", "-","*","/","num","(",")","$","ε"};



//非终结符
void getNonTerminaMap() {
    for (int i=0;i<NONTERMINA_CNT;++i) {
        nonTermina[i].type = 1;
        nonTermina[i].val = s[i];
        unit[s[i]] = i+1;
    }
}

//终结符
void getTerminaMap() {
    for (int i=0;i<TERMINA_CNT;++i) {
        termina[i].type = 2;
        termina[i].val = s2[i];
        unit[s2[i]] = 20+i+1;
    }
}

void init() {
    unit.clear();
    getNonTerminaMap();
    getTerminaMap();
    gotoMap.clear();
}

void splitString(const string& s, vector<string>& v, const string& c) {
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2-pos1));
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length()) v.push_back(s.substr(pos1));
}

void getGrammar(char file[]) {
    freopen(file, "r", stdin);
    char line[1024];
    int cnt = 0;
    Token t;
    while(fgets(line, (sizeof line / sizeof line[0]), stdin)) {
        string ts = line, in;
        if(ts=="\n"||ts==""||ts=="\r") continue;
        vector<string> v;
        in = ts.substr(0,(int)ts.length()-1);
        splitString(in,v," ");
        int sz = (int) v.size();
        int pos;
        string leftString="",rightString="";
        leftString = v[0];

        for(int i=0;i<sz;++i) {
            pos = unit[v[i]];
            if(!pos) continue;
            if(pos>=21) t = termina[pos-21];
            else t = nonTermina[pos-1];
            if(i) {
                gs[cnt].right.push_back(t);
                if(v[i]!="ε") rightString+=" "+v[i];
            }
            else gs[cnt].left.push_back(t);
        }
        gs[cnt].id = cnt;
        gs[cnt].leftString = leftString;
        gs[cnt].rightString = rightString;
        RIGHT[unit[v[0]]-1].push_back(cnt);
        ++cnt;
    }
    fclose(stdin);
}

set<string>::iterator it;
// 计算非终结符的FIRST集合
void calFirst() {
    // 注意ε
    for (int i=0;i<TERMINA_CNT;++i) { // 计算终结符号
        firsts[21+i].v.insert(termina[i].val);
    }
    Token token;
    while(1) {
        bool done = false;
        for (int i=0;i<NONTERMINA_CNT;++i) {
            for (int j=0;j<RIGHT[i].size();++j) {
                bool empty = false;
                for (int k=0;k<gs[RIGHT[i][j]].right.size();++k) {
                    token = gs[RIGHT[i][j]].right[k];
                    if (token.type == 2) {
                        if (!firsts[i+1].v.count(token.val)) {
                            firsts[i+1].v.insert(token.val);
                            done = true;
                        }
                        break;
                    } else {
                        for (it=firsts[unit[token.val]].v.begin();it!=firsts[unit[token.val]].v.end();++it) {
                            if (!firsts[i+1].v.count(*it)) {
                                firsts[i+1].v.insert(*it);
                                done = true;
                            }
                            if (*it == "ε") {
                                empty = true;
                            }
                        }
                    }
                    if (empty == false) break;
                }
                if (empty) {
                    if(!firsts[i+1].v.count("ε")) {
                        firsts[i+1].v.insert("ε");
                        done = true;
                    }
                }
            }
        }
        if (done == false) break;
    }
}

//计算非终结符的FOLLOW集合
void calFollow() {
    string tag = "ε";
    follows[2].v.insert("$");
    while(1) {
        bool done = false;
        for(int i=0;i<GRAMMAR_CNT;++i) {
            int sz = (int)gs[i].right.size();
            for(int j=0;j<sz-1;++j) {
                int now = unit[gs[i].right[j].val];
                int p = j;
                while(p+1 < sz) {
                    int next = unit[gs[i].right[p+1].val];
                    set<string>::iterator it;
                    bool empty = false;
                    for(it=firsts[next].v.begin();it!=firsts[next].v.end();++it) {
                        if(*it == tag) {
                            empty = true;
                            continue;
                        }
                        if(!follows[now].v.count(*it)) {
                            follows[now].v.insert(*it);
                            done = true;
                        }
                    }
                    if(empty == false) break;
                    ++p;
                }
            }
        }
        for(int i=0;i<GRAMMAR_CNT;++i) {
            int sz = (int)gs[i].right.size();
            int left = unit[gs[i].left[0].val];
            int p = sz-1;
            while(p >= 0) {
                bool empty = false;
                int now = unit[gs[i].right[p].val];
                for(it=firsts[now].v.begin();it!=firsts[now].v.end();++it) {
                    if(*it == tag) {
                        empty = true;
                        break;
                    }
                }
                for(it=follows[left].v.begin();it!=follows[left].v.end();++it) {
                    if(!follows[now].v.count(*it)) {
                        follows[now].v.insert(*it);
                        done = true;
                    }
                }
                if(empty == false) break;
                --p;
            }
        }
        if(done == false) break;
    }
}

//扩展当前项集
void extendClosure(vector<pair<Grammar,int> > &c) {
    map<string,int> mp;
    int pos, id;
    Grammar now;
    Token token;
    while(1) {
        int flag = 0;
        for(int i=0;i<(int)c.size();++i) {
            pos = c[i].second;
            now = c[i].first;
            if(pos >= c[i].first.right.size()) continue;
            token = now.right[pos];
            if(token.type != 1) continue;
            id = unit[token.val];
            if(mp[token.val]) continue;
            mp[token.val] = 1;
            flag = 1;
            for(int j=0;j<(int)RIGHT[id-1].size();++j) {
                if(gs[RIGHT[id-1][j]].rightString == "") { // !!!!!!!!!!!!!!!!!!!!!!!!!!!判断推出ε
                    c.push_back(make_pair(gs[RIGHT[id-1][j]] , 1));
                    
                }
                else c.push_back(make_pair(gs[RIGHT[id-1][j]] , 0));
            }
        }
        if(!flag) break;
    }
}

//表格表示的SLR自动机
void printDFA(char file[]) {
    freopen(file,"w",stdout);
    Grammar g;
    string table0 = "-----------------------------------------------------------------------------------------------------------------";
    string table1 = "| 状态 |                      项目集                      |                    后继符号                      |后继状态|";
    cout<<table0<<endl<<table1<<endl;
    for(int i=0;i<=ClosureId;++i) {
        cout<<table0<<endl;
        string state = "S"+to_string(i);
        for(int j=0;j<closures[i].clo.size();++j) {
            
            int pos = closures[i].clo[j].second;
            g = closures[i].clo[j].first;
            string wf = g.leftString+" ->";
            string nextSign, nextState;
            if(closures[i].clo[j].second >= g.right.size()) {
                wf+=g.rightString+"·";
                nextSign = "#"+g.leftString+" ->"+g.rightString;
                //if(g.rightString == "") nextSign+=" ε ";
                nextState = "S"+to_string(ClosureId+1);
            } else {
                for(int k=0;k<g.right.size();++k) {
                    if(k == pos) wf+=" ·"+g.right[k].val;
                    else wf+=" "+g.right[k].val;
                }
                nextSign = g.right[pos].val;
                nextState = "S"+to_string(closures[i].succeed_s[nextSign]);
            }
            cout<<"|"<<setiosflags(ios::left)<<setw(5)<<state;
            cout<<"|"<<setiosflags(ios::left)<<setw(50)<<wf;
            cout<<"|"<<setiosflags(ios::left)<<setw(48)<<nextSign;
            cout<<"|"<<setiosflags(ios::left)<<setw(6)<<nextState<<"|"<<endl;
            state = "";
        }
    }
    cout<<table0<<endl;
    fclose(stdout);
}

map<int,string> from;

int GOTO(int cloId, vector<pair<Grammar,int> > c) {
    int cnt = 0;
    Token now;
    from.clear();
    for(int i=0;i<(int)c.size();++i) {
        if(c[i].first.rightString == "") continue;
        int pos = c[i].second;
        if(pos >= c[i].first.right.size()) continue;
        now = c[i].first.right[pos];
        if(c[i].second+1 > c[i].first.right.size()) { // 已经到最后一个字符了
            continue;
        }
        if(!gotoMap[make_pair(cloId , now.val)]) {
            gotoMap[make_pair(cloId , now.val)] = ++cnt;
            cloTmp[cnt].clo.clear();
            cloTmp[cnt].record.clear();
            from[cnt] = now.val;
        }
        int id = gotoMap[make_pair(cloId , now.val)];
        cloTmp[id].clo.push_back(make_pair(c[i].first , c[i].second+1));
    }
    return cnt;
}

map<vector<pair<int, int> >, int> recordSet;

bool setForm(Closure &clo) {
    for(int i=0;i<(int)clo.clo.size();++i) {
        clo.record.push_back(make_pair(clo.clo[i].first.id, clo.clo[i].second));
    }
    sort(clo.record.begin(),clo.record.end());
    if(recordSet[clo.record]) {
        return false;
    }
    recordSet[clo.record] = ClosureId+1;
    return true;
}

void printFirst() {
    cout<<"=========================================FIRST集========================================="<<endl<<endl;
    for(int i=1;i<21+TERMINA_CNT-1;++i) {
        if(i > NONTERMINA_CNT && i < 21) continue;
        cout<<"FIRST(";
        if(i <= NONTERMINA_CNT) cout<<nonTermina[i-1].val<<") = {";
        else cout<<termina[i-21].val<<") = {";
        string s = "";
        for(it=firsts[i].v.begin();it!=firsts[i].v.end();++it) {
            s+=" "+*it+" , ";
        }
        if(s != "") s = s.substr(0,s.length()-2);
        cout<<s<<"}"<<endl;
    }
    cout<<endl<<"========================================================================================="<<endl<<endl;
}

void printFollow() {
    cout<<"=============================================================FOLLOW集============================================================="<<endl<<endl;
    for(int i=2;i<=NONTERMINA_CNT;++i) {
        cout<<"FOLLOW("<<nonTermina[i-1].val<<") = {";
        string s = "";
        for(it=follows[i].v.begin();it!=follows[i].v.end();++it) {
            s+=" "+*it+" , ";
        }
        if(s != "") s = s.substr(0,s.length()-2);
        cout<<s<<"}"<<endl;
    }
    cout<<endl<<"=================================================================================================================================="<<endl<<endl;
}

string table[70][70];
map<string, int > table_pos;
void setActionAndGoto() {
    Grammar g;
    for(int i=0;i<=ClosureId;++i) {
        for(int j=0;j<closures[i].clo.size();++j) {
            g = closures[i].clo[j].first;
            if(closures[i].clo[j].second >= g.right.size()) {//规约项
                int left = unit[g.left[0].val];
                for(it=follows[left].v.begin();it!=follows[left].v.end();++it) {
                    closures[i].succeed_r[*it] = closures[i].clo[j].first.id;
                }
            }
        }
    }
    for (int i=0;i<=ClosureId;++i) {
        for (int j=0;j<TERMINA_CNT-1;++j) {
            int keyr = closures[i].succeed_r[termina[j].val];
            int keys = closures[i].succeed_s[termina[j].val];
            if(!keyr&&!keys) {
                table[i][j+1] = "";
                continue;
            }
            if(keys) {//移入
                table[i][j+1] = "S"+to_string(keys);
            }
            if(keyr) {//规约
                //if(keys) table[i][j+1] += "||";
                if(!keys) 
                table[i][j+1] += "r"+to_string(keyr);
            }
        }
        for(int j=1;j<NONTERMINA_CNT;++j) {
            int keys = closures[i].succeed_s[nonTermina[j].val];
            if(!keys) {
                table[i][j+TERMINA_CNT-1] = "";
                continue;
            }
            table[i][j+TERMINA_CNT-1] = to_string(keys);
        }
    }
}

void printActionAndGoto(char file[]) {
    freopen(file,"w",stdout);
    string state = "state";
    int cloumn[70];
    for(int i=0;i<70;++i) cloumn[i] = 3;
    for(int i=0;i<=NONTERMINA_CNT+TERMINA_CNT-2;++i) {
        for(int j=0;j<=ClosureId;++j)
            if(table[j][i].length()>cloumn[i]) cloumn[i] = (int)table[j][i].length();
    }
    string line = "-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";
    if(state.length()>cloumn[0]) cloumn[0] = (int)state.length();
    cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[0])<<"state";
    
    for(int i=0;i<TERMINA_CNT-1;++i) {
        table_pos[termina[i].val] = i+1;
        if(termina[i].val.length()>cloumn[i+1]) cloumn[i+1] = (int)termina[i].val.length();
        cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[i+1])<<termina[i].val; //cnt++;
    }
    for(int i=1;i<NONTERMINA_CNT;++i) {
        table_pos[nonTermina[i].val] = i-1+TERMINA_CNT;
        if(nonTermina[i].val.length()>cloumn[i-1+TERMINA_CNT]) cloumn[i-1+TERMINA_CNT] = (int)nonTermina[i].val.length();
        cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[i-1+TERMINA_CNT])<<nonTermina[i].val; //cnt++;
    }
    cout<<"|"<<endl;
    table[1][table_pos["$"]] = "acc";
    
    for(int i=0;i<=ClosureId;++i) {
        cout<<line<<endl;
        cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[0])<<to_string(i);
        for(int j=0;j<TERMINA_CNT-1;++j) {
            cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[j+1])<<table[i][j+1];
        }
        
        for(int j=1;j<NONTERMINA_CNT;++j) {
            cout<<"|"<<setiosflags(ios::left)<<setw(cloumn[j-1+TERMINA_CNT])<<table[i][j-1+TERMINA_CNT];
        }
        cout<<"|"<<endl;
    }
    fclose(stdout);
}

void getCanonical() {
    int isCalGoto[100];
    memset(isCalGoto, 0, sizeof isCalGoto);
    vector<pair<Grammar,int> > c;
    c.push_back(make_pair(gs[0], 0));
    closures[0].clo = c;
    closures[0].id = 0;
    extendClosure(closures[0].clo);
    setForm(closures[0]);
    while(1) {
        int flag = 1;
        for(int i=0;i<=ClosureId;++i) {
            if(!isCalGoto[i]) {
                flag = 0;
                isCalGoto[i] = 1;
                int cnt = GOTO(i , closures[i].clo);
                for(int a=1;a<=cnt;++a) {
                    extendClosure(cloTmp[a].clo);
                    // 设置项目集组成
                    if(setForm(cloTmp[a]) == false) {
                        closures[i].succeed_s[from[a]] = recordSet[cloTmp[a].record];
                        continue;
                    }
                    closures[++ClosureId] = cloTmp[a];
                    closures[ClosureId].id = ClosureId;
                    gotoMap[make_pair(i, from[a])] = ClosureId;
                    closures[i].succeed_s[from[a]] = ClosureId;
                }
            }
        }
        if(flag) break;
    }
}
string inputQueue[1005];
string symbolStack[1005];
int stateStack[1005];
int inputQueueTail;
void readInput(char file[]) {
    freopen(file,"r",stdin);
    string s;
    int cnt = 0;
    while(cin>>s) {
        inputQueue[++cnt] = s;
    }
    inputQueue[++cnt] = "$";
    inputQueueTail = cnt;
    fclose(stdin);
}
int cntIdTop;
int cntNumTop;
vector<string> idTop;
vector<string> numTop;
void readTop(char file[]) {
	FILE *fp = fopen(file, "r");

	char ty[11], val[11];
	while(~fscanf(fp, "%s%s", ty, val)) {
		if(ty[0]=='i'&&ty[1]=='d') {
			idTop.push_back(val);
		} else {
			numTop.push_back(val);
		}
	}

	fclose(fp);
}
void printError(char file[],int x) {
	freopen(file, "w", stdout);
	cout<<"error"<<endl;
	fclose(stdout);
}

const int N=1010;
int cntAddr, cntCode;
string code[N], valCode[N], addr[N];

void getCode(int l,int r,int num) { 
	if(num==0) {
		//code[l]=code[l];
	} else if(num==1) {
		code[l]=code[r-1];
	} else if(num==2) {
		code[l]=code[r-1];
	} else if(num==3) {
		code[l]="";
	} else if(num==4) {
		code[l]="";
	} else if(num==5) {
		code[l]=code[r-1];
	} else if(num==6) {
		//code[l]=code[l];
	} else if(num==7) {
		code[l]="";
	} else if(num==8) {
		//code[l]=code[l];
	} else if(num==9) {
		code[l]+=code[r];
	} else if(num==10) {
		valCode[++cntCode]=addr[l] + " = " + addr[r];
		char t=cntCode;
		code[l]=code[r]+t;
	} else if(num==11) {
		//code[l]=code[l];
	} else if(num==12) {
		int sz=code[l+1].size();
		char t1=code[l+1][sz-2];
		char t2=code[l+1][sz-1];
		valCode[t1]+=to_string((int)code[r][0])+"L";
		valCode[t2]+=to_string(cntCode+1)+"L";
		code[l]=code[l+1]+code[r];
	} else if(num==13) {
		int sz=code[l+1].size();
		char t1=code[l+1][sz-2];
		char t2=code[l+1][sz-1];
		valCode[t1]+=to_string((int)code[r-2][0])+"L";
		valCode[t2]+=to_string((int)code[r][0])+"L";
		++cntCode;
		valCode[cntCode]="goto "+to_string(cntCode+1)+"L";
		char t=cntCode;
		code[l]=code[l+1]+code[r-2]+t+code[r];
	} else if(num==14) {
		int sz=code[l+1].size();
		char t1=code[l+1][sz-2];
		char t2=code[l+1][sz-1];
		valCode[t1]+=to_string((int)code[r][0])+"L";//code[r][0]+'0';
		valCode[t2]+=to_string(cntCode+1)+"L";//cntCode+1+'0';
		code[l]=code[l+1]+code[r];
		valCode[++cntCode]="goto "+to_string((int)code[l][0])+"L";//(code[l][0]+'0');
		code[l]+=cntCode;
	} else if(num==15) {
		int sz=code[r].size();
		char t1=code[r][sz-2];
		char t2=code[r][sz-1];
		valCode[t1]+=to_string(cntCode+1)+"L";//cntCode+1+'0';
		valCode[t2]+=to_string((int)code[l+1][0])+"L";//code[l+1][0]+'0';
		code[l]=code[l+1]+code[r];
	} else if(num==16) {
		valCode[++cntCode]="if "+addr[l]+" < "+addr[r]+" goto ";
		char t1=cntCode;
		valCode[++cntCode]="goto ";
		char t2=cntCode;
		code[l]=code[l]+code[r]+t1+t2;
	} else if(num==17) {
		string newTemp="t"+to_string(++cntAddr);
		valCode[++cntCode]=newTemp+" = "+addr[l]+" + "+addr[r];
		char t=cntCode;
		code[l]=code[l]+code[r]+t;
		addr[l]=newTemp;
	} else if(num==18) {
		string newTemp="t"+to_string(++cntAddr);
		valCode[++cntCode]=newTemp+" = "+addr[l]+" - "+addr[r];
		char t=cntCode;
		code[l]=code[l]+code[r]+t;
		addr[l]=newTemp;
	} else if(num==19) {
	} else if(num==20) {
		string newTemp="t"+to_string(++cntAddr);
		valCode[++cntCode]=newTemp+" = "+addr[l]+" * "+addr[r];
		char t=cntCode;
		code[l]=code[l]+code[r]+t;
		addr[l]=newTemp;
	} else if(num==21) {
		string newTemp="t"+to_string(++cntAddr);
		valCode[++cntCode]=newTemp+" = "+addr[l]+" / "+addr[r];
		char t=cntCode;
		code[l]=code[l]+code[r]+t;
		addr[l]=newTemp;
	} else if(num==22) {
	} else if(num==23) {
		code[l]="";
	} else if(num==24) {
		code[l]="";
	}
	/*
	for(int i=0;i<(int)code[l].size();++i) {
		cout<<(int)code[l][i]<<": "<<valCode[code[l][i]]<<endl;
	}
	cout<<to_string(cntCode+1)<<": "<<endl;
	*/
}


string programeTable[1005][5];
void solve(char outputFile[],char errorFile[]) {

	cntAddr = cntCode = 0;

	string s;
	freopen(outputFile, "w", stdout);
	int stateStackTop = 0, inputQueueTop = 0, symbolStackTop = 0;
	stateStack[++stateStackTop] = 0;
	int a = table_pos[inputQueue[++inputQueueTop]];
	int cnt = 0;
	while(1) {
		++cnt;
		if(stateStackTop<=0) {
			printError(errorFile,1);
			break;
		}
		int top = stateStack[stateStackTop];
		programeTable[cnt][0] = "("+to_string(cnt)+")";
		programeTable[cnt][1] = "stateStack: ";
		for(int i=1;i<=stateStackTop;++i) {
			programeTable[cnt][1] += to_string(stateStack[i])+" ";
		}
		programeTable[cnt][2] = "symbolStack: ";
		for(int i=1;i<=symbolStackTop;++i) {
			programeTable[cnt][2] += symbolStack[i]+" ";
		}
		programeTable[cnt][3] = "INPUT: ";
		for(int i=inputQueueTop;i<=inputQueueTail;++i) {
			programeTable[cnt][3] += inputQueue[i]+" ";
		}
		s = table[top][a];
		if(s == "") {
			printError(errorFile,2);
			break;
		}
		if(s == "acc") {
			programeTable[cnt][4] = "accept";
			cout<<programeTable[cnt][0]<<"\n"<<programeTable[cnt][1]<<"\n"<<programeTable[cnt][2]<<"\n"<<programeTable[cnt][3]<<"\n"<<programeTable[cnt][4]<<endl;
			break;
		}
		else if(s[0] == 's'|| s[0] == 'S') {//移入
			int num = 0;
			for(int i=1;i<s.length();++i) {
				if(s[i]=='|') break;
				num = num*10+(s[i]-'0');
			}
			stateStack[++stateStackTop] = num;
			symbolStack[++symbolStackTop] = inputQueue[inputQueueTop];

			//
			string tmpSym = inputQueue[inputQueueTop];
			int tmpPoint = symbolStackTop;
			if(tmpSym=="id") {
				addr[tmpPoint] = idTop[cntIdTop++];
			} else if(tmpSym=="num") {
				addr[tmpPoint] = numTop[cntNumTop++];
			}

			string next = inputQueue[++inputQueueTop];
			a = table_pos[next];
			programeTable[cnt][4] = "移入";
		} else if(s[0] == 'r' || s[0] == 'R'){//规约
			int num = 0;
			for(int i=1;i<s.length();++i) {
				if(s[i] == '|') break;
				num = num*10+(s[i]-'0');
			}
			int right = (int)gs[num].right.size();
			if(symbolStackTop<right||stateStackTop<right) {
				printError(errorFile,3);
				break;
			}
			if(gs[num].rightString != "") {//需要消耗输入
				stateStackTop -= right;
				symbolStackTop -= right;
			}

			//
			getCode(symbolStackTop+1, symbolStackTop+right, num);

			symbolStack[++symbolStackTop] = gs[num].left[0].val;
			top = stateStack[stateStackTop];
			string tmp = table[top][table_pos[gs[num].left[0].val]];
			int t = 0;
			for(int h=0;h<tmp.length();++h) {
				t = t*10+(tmp[h]-'0');
			}
			stateStack[++stateStackTop] = t;
			programeTable[cnt][4] = "根据"+gs[num].leftString+"->"+gs[num].rightString+"规约";
		}
		cout<<programeTable[cnt][0]<<"\n"<<programeTable[cnt][1]<<"\n"<<programeTable[cnt][2]<<"\n"<<programeTable[cnt][3]<<"\n"<<programeTable[cnt][4]<<endl<<endl;
	}
	fclose(stdout);
}
void print(char file[]) {
	freopen(file, "w", stdout);

	/*
	for(int i=0;i<(int)code[1].size();++i) {
		int ind=code[1][i];
		cout<<ind<<": "<<valCode[ind]<<endl;
	}
	cout<<to_string(cntCode+1)<<": "<<endl;
	*/

	map<string,int> mp;
	for(int i=0;i<(int)code[1].size();++i) {
		int j=code[1][i];
		mp[to_string(j)+"L"]=i;
	}
	mp[to_string(cntCode+1)+"L"]=code[1].size();

	for(int i=0;i<(int)code[1].size();++i) {
		cout<<i<<": ";
		int j=code[1][i];
		stringstream ss(valCode[j]);
		string s;
		while(ss>>s) {
			if(mp.count(s)) cout<<mp[s]<<" ";
			else cout<<s<<" ";
		}
		cout<<endl;
	}
	cout<<(int)code[1].size()<<":"<<endl;

	fclose(stdout);
}
int main() {
    char gramarFile[] = "grammar.txt";
    char outputFile[] = "output.txt";
    char actionAndGotoFile[] = "actionAndGoto.txt";
    char DFAFile[] = "slr.txt";
    char errorFile[] = "error.txt";
    char inputFile[] = "input.txt";
    char topFile[] = "top.txt";
    char codeFile[] = "ir_code.txt";


	init();
	getGrammar(gramarFile);
	getCanonical();

	calFirst();
	printFirst();
	calFollow();
	printFollow();

	printDFA(DFAFile);
	setActionAndGoto();
	printActionAndGoto(actionAndGotoFile);
	readInput(inputFile);
	readTop(topFile);
	solve(outputFile, errorFile);
	print(codeFile);
	return 0;
}

