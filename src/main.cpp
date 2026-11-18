#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct User { string name, email, password; bool admin = false; };
struct Complaint {
    int id = 0;
    string email, title, description, category, priority, location, status, created;
};

const string USERS_FILE = "data/users.csv";
const string COMPLAINTS_FILE = "data/complaints.csv";
vector<User> users;
vector<Complaint> complaints;

string lower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(tolower(c)); });
    return s;
}
string trim(const string& s) {
    size_t a=s.find_first_not_of(" \t\r\n"), b=s.find_last_not_of(" \t\r\n");
    return a==string::npos ? "" : s.substr(a,b-a+1);
}
string readLine(const string& prompt) { cout << prompt; string s; getline(cin,s); return trim(s); }
string csvEscape(const string& s) {
    string out="\"";
    for(char c:s) { if(c=='"') out+='"'; out+=c; }
    return out+'"';
}
vector<string> parseCsv(const string& line) {
    vector<string> out; string field; bool quoted=false;
    for(size_t i=0;i<line.size();++i) {
        char c=line[i];
        if(c=='"') { if(quoted && i+1<line.size() && line[i+1]=='"'){field+='"';++i;} else quoted=!quoted; }
        else if(c==',' && !quoted){out.push_back(field);field.clear();}
        else field+=c;
    }
    out.push_back(field); return out;
}
void loadData() {
    ifstream uf(USERS_FILE); string line;
    while(getline(uf,line)){auto f=parseCsv(line); if(f.size()==4) users.push_back({f[0],f[1],f[2],f[3]=="1"});}
    ifstream cf(COMPLAINTS_FILE);
    while(getline(cf,line)){auto f=parseCsv(line); if(f.size()==9){Complaint c; c.id=stoi(f[0]);c.email=f[1];c.title=f[2];c.description=f[3];c.category=f[4];c.priority=f[5];c.location=f[6];c.status=f[7];c.created=f[8];complaints.push_back(c);}}
}
void saveUsers(){ofstream o(USERS_FILE);for(auto&u:users)o<<csvEscape(u.name)<<','<<csvEscape(u.email)<<','<<csvEscape(u.password)<<','<<(u.admin?1:0)<<'\n';}
void saveComplaints(){ofstream o(COMPLAINTS_FILE);for(auto&c:complaints)o<<c.id<<','<<csvEscape(c.email)<<','<<csvEscape(c.title)<<','<<csvEscape(c.description)<<','<<csvEscape(c.category)<<','<<csvEscape(c.priority)<<','<<csvEscape(c.location)<<','<<csvEscape(c.status)<<','<<csvEscape(c.created)<<'\n';}
bool containsAny(const string&s,const vector<string>& words){for(auto&w:words)if(s.find(w)!=string::npos)return true;return false;}
pair<string,string> classify(const string& raw) {
    string s=lower(raw); map<string,vector<string>> keys={
      {"Internet / Wi-Fi",{"wifi","wi-fi","internet","network","router","connectivity"}},
      {"Electrical",{"electric","electricity","power","light","fan","switch","socket","sparking"}},
      {"Water Supply",{"water","tap","drinking","leak","pipeline"}},
      {"Cleaning",{"dirty","clean","garbage","dust","waste","toilet","washroom","sanitation"}},
      {"Infrastructure",{"broken","damaged","desk","chair","bench","classroom","roof","door","window","projector"}},
      {"Security",{"security","unsafe","theft","harassment","guard","safety"}}
    };
    string category="Other"; size_t best=0;
    for(auto& entry:keys){size_t score=0;for(auto&w:entry.second){size_t p=0;while((p=s.find(w,p))!=string::npos){++score;p+=w.size();}}if(score>best){best=score;category=entry.first;}}
    bool high=containsAny(s,{"fire","smoke","sparking","short circuit","danger","emergency","injury","accident"});
    bool medium=containsAny(s,{"urgent","today","exam","lab","not working","broken"});
    return {category,high?"High":(medium?"Medium":"Low")};
}
string nowText(){return "Submitted in current session";}
User* findUser(const string& email){for(auto&u:users)if(lower(u.email)==lower(email))return &u;return nullptr;}
void registerStudent(){
    cout<<"\n--- Student registration ---\n";string name=readLine("Full name: "),email=readLine("Email: "),pw=readLine("Password (8+ chars): ");
    if(name.empty()||email.find('@')==string::npos||pw.size()<8){cout<<"Please enter valid details; password needs 8 characters.\n";return;}
    if(findUser(email)){cout<<"Email already registered.\n";return;}
    users.push_back({name,email,pw,false});saveUsers();cout<<"Account created. You can now sign in.\n";
}
void showComplaint(const Complaint&c){cout<<"\n#"<<c.id<<" | "<<c.title<<"\n  Category: "<<c.category<<" | Priority: "<<c.priority<<" | Status: "<<c.status<<"\n  Location: "<<c.location<<"\n  Description: "<<c.description<<"\n  "<<c.created<<"\n";}
void submitComplaint(const User&u){
    cout<<"\n--- Submit campus issue ---\n";Complaint c;c.id=1;for(auto&x:complaints)c.id=max(c.id,x.id+1);
    c.email=u.email;c.title=readLine("Short title: ");c.description=readLine("Describe the issue: ");c.location=readLine("Campus location: ");
    if(c.title.empty()||c.description.size()<10||c.location.empty()){cout<<"Please fill every field; description must be at least 10 characters.\n";return;}
    auto result=classify(c.title+" "+c.description);c.category=result.first;c.priority=result.second;c.status="Pending";c.created=nowText();complaints.push_back(c);saveComplaints();
    cout<<"\nReport submitted successfully!\nAI category: "<<c.category<<"\nEstimated priority: "<<c.priority<<"\n";
}
void studentMenu(const User&u){while(true){cout<<"\n=== STUDENT DASHBOARD: "<<u.name<<" ===\n1. Submit complaint\n2. View my complaints\n0. Logout\n";string ch=readLine("Choose: ");if(ch=="1")submitComplaint(u);else if(ch=="2"){bool any=false;for(auto&c:complaints)if(c.email==u.email){showComplaint(c);any=true;}if(!any)cout<<"No complaints submitted yet.\n";}else if(ch=="0")break;else cout<<"Invalid choice.\n";}}
void adminMenu(){while(true){int pending=0,progress=0,resolved=0;map<string,int> cats;for(auto&c:complaints){if(c.status=="Pending")pending++;else if(c.status=="In Progress")progress++;else resolved++;cats[c.category]++;}
    cout<<"\n=== ADMIN DASHBOARD ===\nTotal: "<<complaints.size()<<" | Pending: "<<pending<<" | In progress: "<<progress<<" | Resolved: "<<resolved<<"\n1. View all complaints\n2. Update complaint status\n3. Category summary\n0. Logout\n";
    string ch=readLine("Choose: ");if(ch=="1"){if(complaints.empty())cout<<"No complaints yet.\n";for(auto&c:complaints)showComplaint(c);}
    else if(ch=="2"){string idstr=readLine("Complaint ID: ");int id=0;try{id=stoi(idstr);}catch(...){cout<<"Invalid ID.\n";continue;}Complaint* target=nullptr;for(auto&c:complaints)if(c.id==id)target=&c;if(!target){cout<<"Complaint not found.\n";continue;}cout<<"1. Pending  2. In Progress  3. Resolved\n";string s=readLine("New status: ");if(s=="1")target->status="Pending";else if(s=="2")target->status="In Progress";else if(s=="3")target->status="Resolved";else{cout<<"Invalid status.\n";continue;}saveComplaints();cout<<"Status updated.\n";}
    else if(ch=="3"){if(cats.empty())cout<<"No data.\n";for(auto&p:cats)cout<<"  "<<left<<setw(22)<<p.first<<p.second<<"\n";}
    else if(ch=="0")break;else cout<<"Invalid choice.\n";
}}
int main(){
    loadData();
    if(!findUser("admin@campusassist.local")){users.push_back({"Campus Admin","admin@campusassist.local","Admin@123",true});saveUsers();}
    cout<<"\n====================================\n       CAMPUSASSIST AI (C++)\n Campus Issue Reporting & Resolution\n====================================\n";
    while(true){cout<<"\n1. Student sign in\n2. Student registration\n3. Admin sign in\n0. Exit\n";string ch=readLine("Choose: ");
      if(ch=="1"||ch=="3"){string email=readLine("Email: "),pw=readLine("Password: ");User*u=findUser(email);if(!u||u->password!=pw||u->admin!=(ch=="3")){cout<<"Invalid login.\n";continue;}if(u->admin)adminMenu();else studentMenu(*u);}
      else if(ch=="2")registerStudent();else if(ch=="0"){cout<<"Goodbye!\n";break;}else cout<<"Invalid choice.\n";
    }
    return 0;
}
