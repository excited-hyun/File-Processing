#define _CRT_SECURE_NO_WARNINGS
#include "Purchase.h"
#include "recfile.h"
#include <sstream>
#include "btree.h"
#include "btnode.h"


extern BTree <char> bt;
extern Purchase* purchase_arr;
extern char world_key;

//생성자
Purchase::Purchase() {

}
Purchase::Purchase(const char* new_purchase_id) {
	Purchase();
	update_purchase_id(new_purchase_id);
}

//복사 생성자
Purchase::Purchase(const Purchase& p) {
	Purchase();

	update_purchase_id(p.purchase_id);
	update_game_id(p.game_id);
	update_member_id(p.member_id);
	update_purchase_date(p.purchase_date);
	update_key(p.key);
}

//객체 복사 생성자 (대입연산자)
Purchase& Purchase::operator = (const Purchase& p) {
	update_purchase_id(p.purchase_id);
	update_game_id(p.game_id);
	update_member_id(p.member_id);
	update_purchase_date(p.purchase_date);
	update_key(p.key);

	return *this;
}

//연산자 오버로딩
bool Purchase::operator == (const Purchase& p) {
	return !memcmp(purchase_id, p.purchase_id, LENPID);
}
bool Purchase::operator != (const Purchase& p) {
	if (purchase_id != p.purchase_id) {
		return true;
	}
	return false;
}

istream& operator >> (istream& is, Purchase& g) {
	char str[100];
	char* temp;
	is.getline(str, 100);
	//printf("%s\n", str);

	temp = strtok(str, "|");
	g.update_purchase_id(temp);

	temp = strtok(NULL, "|");
	g.update_game_id(temp);

	temp = strtok(NULL, "|");
	g.update_member_id(temp);

	temp = strtok(NULL, "\n");
	g.update_purchase_date(temp);

	return is;
}

ostream& operator << (ostream& os, Purchase& p) {
	char purchase_id[LENPID + 1];
	char game_id[LENGID + 1];
	char pur_date[LENDATE + 1];

	strncpy(purchase_id, p.purchase_id, LENPID);
	purchase_id[LENPID] = '\0';

	strncpy(game_id, p.game_id, LENGID);
	game_id[LENGID] = '\0';

	strncpy(pur_date, p.purchase_date, LENDATE);
	pur_date[LENDATE] = '\0';
	char temp[2];
	temp[0] = p.key;
	temp[1] = '\0';
	string pKey(temp, 2);

	os << "PURCHASE ID: " << purchase_id << endl;
	os << "GAME ID: " << game_id << endl;
	os << "MEMBER ID: " << p.member_id << endl;
	os << "PURCHASE DATE: " << pur_date << endl;
	os << "KEY: " << pKey << endl;
	//printf("%c\n", p.key);

	return os;
}

void Purchase::update_purchase_id(const char* new_purchase_id) {
	memcpy(purchase_id, new_purchase_id, LENPID);
}

void Purchase::update_game_id(const char* new_game_id) {
	memcpy(game_id, new_game_id, LENGID);
}
void Purchase::update_member_id(string new_Purchase_id) {
	member_id = new_Purchase_id;
}

void Purchase::update_purchase_date(const char* new_purchase_date) {
	memcpy(purchase_date, new_purchase_date, LENDATE);
}

void Purchase::update_key(const char new_key) {
	key = new_key;
}

void showPurchase() {
	ifstream ifs("listOfPurchase.txt");
	if (ifs.fail()) {
		cout << "File Open error!" << endl;
		return;
	}

	int n;
	ifs >> n;
	ifs.ignore(numeric_limits<streamsize>::max(), '\n');
	//cout << n;

	//vector<Purchase> m_list;

	for (int i = 0; i < n; i++) {
		//cout << i << endl;
		Purchase p;
		ifs >> p;
		if (i < 10)
			cout << p << endl;
		//m_list.push_back(m);
	}


	return;
}


bool Purchase::Pack(IOBuffer& Buffer) const {
	int numBytes;

	Buffer.Clear();

	string p_purchase_id(purchase_id, LENPID);
	string p_game_id(game_id, LENGID);
	string p_purchas_date(purchase_date, LENDATE);

	numBytes = Buffer.Pack(p_purchase_id.c_str());
	if (numBytes == -1) return false;
	numBytes = Buffer.Pack(p_game_id.c_str());
	if (numBytes == -1) return false;
	numBytes = Buffer.Pack(member_id.c_str());
	if (numBytes == -1) return false;
	numBytes = Buffer.Pack(p_purchas_date.c_str());
	if (numBytes == -1) return false;
	char temp[2];
	temp[0] = key;
	temp[1] = '\0';
	string p_key(temp, 1);
	numBytes = Buffer.Pack(p_key.c_str());
	if (numBytes == -1) return false;
	return true;
}


bool Purchase::Unpack(IOBuffer& Buffer) {
	int numBytes;

	char buf[STDMAXBUF];
	char temp[STDMAXBUF];

	numBytes = Buffer.Unpack(purchase_id, LENPID);
	if (numBytes == -1) return false;

	numBytes = Buffer.Unpack(game_id, LENGID);
	if (numBytes == -1) return false;

	numBytes = Buffer.Unpack(buf);
	if (numBytes == -1) return false;
	member_id = buf;

	numBytes = Buffer.Unpack(purchase_date, LENDATE);
	if (numBytes == -1) return false;

	numBytes = Buffer.Unpack(temp, 2);
	key = temp[0];
	if (numBytes == -1) return false;

	return true;
}

void PurchaseTest() {
	ifstream ifs("listOfPurchase.txt");
	if (ifs.fail()) {
		cout << "File Open error!" << endl;
		return;
	}

	int n;
	ifs >> n;
	//printf("%d\n", n);
	ifs.ignore(numeric_limits<streamsize>::max(), '\n');

	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	char filename[20] = "fileOfPurchase.dat";
	//Write test
	PurchaseFile.Create(filename, ios::out | ios::trunc);
	for (int i = 0; i < n; i++) {
		Purchase p;
		ifs >> p;
		int recaddr;
		if ((recaddr = PurchaseFile.Write(p)) == -1) {
			cout << "Write Error!" << endl;
		}
	}

	PurchaseFile.Close();

	//Read Test
	PurchaseFile.Open(filename, ios::in);
	for (int i = 0; i < n; i++) {
		Purchase p;
		PurchaseFile.Read(p);
		if (i < 10)
			cout << p << endl;
	}
	PurchaseFile.Close();

	return;
}


int purchaseRecordCount() {
	int cnt = 0;
	Purchase p;
	char file_name[20] = "fileOfPurchase.dat";

	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);
	PurchaseFile.Open(file_name, ios::in);
	while (PurchaseFile.Read(p) != -1)
		cnt++;
	PurchaseFile.Close();
	return cnt;
}

int searchPurchase(char* PurchaseID) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);
	Purchase key(PurchaseID);
	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		if (p == PurchaseID) {
			find = addr;
			break;
		}
	}
	PurchaseFile.Close();
	return find;
}

int searchPurchase(char* PurchaseID, Purchase* pur) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		if (p == PurchaseID) {
			find = addr;
			*pur = p;
			break;
		}
	}
	PurchaseFile.Close();
	return find;
}

int searchGameID(char* GameID) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getGameID();
		
		if (!strncmp(GameID, id, LENGID)) {
			find = addr;
			cout << p << endl;
		}
	}
	PurchaseFile.Close();
	return find;
}
int searchGameID(char* GameID, Purchase* pur) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getGameID();
		//cout << id << "!" << endl;
		if (!strncmp(GameID, id, LENGID)) {
			find = addr;
			*pur = p;
			cout << p << endl;
		}
	}
	PurchaseFile.Close();
	return find;
}
int searchMemberID(char* MemberID) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	string id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getMemberID();
		if (MemberID== id) {
			find = addr;
			cout << p << endl;
		}
	}
	PurchaseFile.Close();
	return find;
}
int searchMemberID(char* MemberID, Purchase* pur) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	string id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getMemberID();
		if (MemberID == id) {
			find = addr;
			*pur = p;
			cout << p << endl;
		}
	}
	PurchaseFile.Close();
	return find;
}

void insertPurchase(char* PurchaseID) {
	Purchase p;
	string buf;
	char* update_temp = new char(100);

	p.update_purchase_id(PurchaseID);

	cout << "Game ID(8char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 8) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_game_id(update_temp);

	cout << "Member ID: ";
	cin >> buf;
	p.update_member_id(buf);

	cout << "Purchase Date(10char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 10) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_purchase_date(update_temp);
	if (world_key < 1 || world_key > 127) {
		cout << "Exeeded Key limit" << endl;
		return;
	}
	p.update_key(world_key);
	
	cout << "new Key : " << world_key << endl;
	world_key++;
	int recaddr;
	char file_name[20] = "fileOfPurchase.dat";

	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);
	PurchaseFile.Open(file_name, ios::in | ios::out | ios::app);

	int index = 0;

	while (1) {
		Purchase p_temp;
		if (PurchaseFile.Read(p_temp) == -1)
			break;
		purchase_arr[index] = p_temp;
		index += 1;
	}

	PurchaseFile.Close();
	char file_name1[20] = "fileOfPurchase.dat";
	PurchaseFile.Create(file_name1, ios::out | ios::trunc);

	for (int i = 0; i < index; i++) {
		recaddr = PurchaseFile.Write(purchase_arr[i]);
	}

	recaddr = PurchaseFile.Write(p);		
	PurchaseFile.Close();

	///-----------------------------------

	bt.Insert(p.Key(), recaddr);			
}

void updatePurchase(char* PurchaseID) {
	int num = purchaseRecordCount();
	ifstream ifs("fileOfPurchase.dat");
	string filename;
	char file_name[20] = "fileOfPurchase.dat";
	int index = 0;
	int up_index = 0;
	Purchase p;
	int ifs_last;
	int find_ID = 0;
	DelimFieldBuffer buffer('|', 1000);
	RecordFile <Purchase> PurchaseFile(buffer);
	Purchase* purchase_arr = new Purchase[100000];

	if (ifs.fail()) {
		cout << "fileOfPurchase.dat open error!" << endl;
		return;
	}
	ifs.ignore(numeric_limits<streamsize>::max(), '\n');

	PurchaseFile.Open(file_name, ios::in);
	ifs.clear();	//because of c++98
	ifs.seekg(0, ifs.end);
	ifs_last = ifs.tellg();
	ifs.seekg(0, ifs.beg);
	for (int i = 0; i < num; i++) {
		PurchaseFile.Read(p);
		//cout << p.getPurchaseID() << " error! " << PurchaseID << endl;
		if (strncmp(p.getPurchaseID(), PurchaseID, 12) == 0) {
			up_index = i;
			purchase_arr[i] = p;
			//ifs.seekg(p.get_length(), ifs.cur);
			//break;
		}
		else {
			purchase_arr[i] = p;
		}
		//ifs.seekg(p.get_length(), ifs.cur);
	}
	p = purchase_arr[up_index];
	string temp;
	char* update_temp = new char(100);

	cout << "++++++++++++++++++++++++" << endl;
	cout << "input new information" << endl;

	cout << "new Game ID(8char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 8) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_game_id(update_temp);

	cout << "new Member ID: ";
	cin >> temp;
	p.update_member_id(temp);

	cout << "new Purchase Date(10char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 10) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_purchase_date(update_temp);

	ifs.seekg(p.get_length(), ifs.cur);
	cout << endl;
	cout << "Update success!!!" << endl;
	cout << p;


	purchase_arr[up_index] = p;
	ifs.seekg(p.get_length(), ifs.cur);

	PurchaseFile.Close();

	PurchaseFile.Create(file_name, ios::out | ios::trunc);
	int recaddr;
	for (int i = 0; i < num; i++) {
		if ((recaddr = PurchaseFile.Write(purchase_arr[i])) == -1)
			cout << "Write Error!" << endl;
	}
	PurchaseFile.Close();

	ifs.close();
}

void deletePurchase(char* PurchaseID) {
	int num = purchaseRecordCount();
	int found = 0;
	ifstream ifs("fileOfPurchase.dat");
	char file_name[20] = "fileOfPurchase.dat";
	int index = 0;
	Purchase p;
	Purchase* Purchase_arr = new Purchase[100000];
	int ifs_last;

	if (ifs.fail()) {
		cout << "File Open error!" << endl;
		return;
	}

	ifs.ignore(numeric_limits<streamsize>::max(), '\n');
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);
	ifs.clear();
	ifs.seekg(0, ifs.end);
	ifs_last = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	for(int i=0; i<num; i++ ){
		PurchaseFile.Read(p);
		//cout << p.getPurchaseID() << " error! " << PurchaseID << endl;
		if (strncmp(p.getPurchaseID(), PurchaseID, 12) == 0) {
			found = 1;
			//ifs.seekg(p.get_length(), ifs.cur);
			//break;
		}
		else {
			Purchase_arr[index] = p;
			index += 1;
		}
		//ifs.seekg(p.get_length(), ifs.cur);
	}
	PurchaseFile.Close();


	if (found) {
		PurchaseFile.Create(file_name, ios::out | ios::trunc);
		int recaddr;
		for (int i = 0; i < index; i++) {
			if ((recaddr = PurchaseFile.Write(Purchase_arr[i])) == -1)
				cout << "Write Error!" << endl;
		}
		PurchaseFile.Close();
	}
	ifs.close();
}

void deleteMemPurchase(char* MemberID) {
	char p_id[13];
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	string id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getMemberID();
		if (MemberID == id) {
			find = addr;
			strncpy(p_id, p.getPurchaseID(), 12);
			p_id[12] = '\0';
			deletePurchase(p_id);
		}
	}
	PurchaseFile.Close();
}
void deleteGamePurchase(char* GameID) {
	char p_id[13];
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* id;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getGameID();

		if (!strncmp(GameID, id, LENGID)) {
			find = addr;
			strncpy(p_id, p.getPurchaseID(), 12);
			p_id[12] = '\0';
			deletePurchase(p_id);
		}
	}
	PurchaseFile.Close();
}

int searchPurchase_Customer(char* rid, Purchase* res, char AccessID[]) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* ID;
	string memID;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);
	bool flag = false;
	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		ID = p.getPurchaseID();
		memID = p.getMemberID();
		//cout << ID << endl;
		if (!strncmp(ID, rid, 12) && !strcmp(AccessID, memID.c_str())) {
			find = addr;
			*res = p;
			cout << p << endl;
			flag = true;
		}

	}

	PurchaseFile.Close();
	return find;

}
int searchGame_Customer(char* gid, Purchase* res, char AccessID[]) {
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* id;
	string memID;

	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		id = p.getGameID();
		memID = p.getMemberID();
		//cout << id << "!" << endl;
		if (!strncmp(gid, id, LENGID) && !strcmp(AccessID, memID.c_str())) {
			find = addr;
			*res = p;
			cout << p << endl;
		}
	}
	PurchaseFile.Close();
	return find;
}

void showAllPurchase_Customer(char AccessID[]) {
	char* ID;
	string memID;

	int n = purchaseRecordCount();
	int addr, retValue = -1;

	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);
	bool flag = false;


	char file_name[20] = "fileOfPurchase.dat";
	PurchaseFile.Open(file_name, ios::in);

	for (int i = 0; i < n; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		ID = p.getPurchaseID();
		memID = p.getMemberID();
		if (!strcmp(AccessID, memID.c_str())) {
			retValue = addr;
			cout << p << endl;
			flag = true;
		}
	}

	if (!flag)
		cout << "Not Founded" << endl;
	PurchaseFile.Close();
	return;
}

void insertPurchase_Customer(char* recID, char AccessID[]) {
	Purchase p;
	bool success;
	string buf;

	char* update_temp = new char(100);

	string id(recID);
	if (id.length() != 12) {
		cout << "Purchase ID length Wrong!" << endl;
		cin.ignore();
		cin.get();
		return;
	}
	p.update_purchase_id(recID);
	
	cout << "Game ID(8char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 8) {
		cout << "Wrong length!" << endl;
		return;
	}
	p.update_game_id(update_temp);

	p.update_member_id(AccessID);

	cout << "Purchase Date(10char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 10) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_purchase_date(update_temp);

	char file_name[20] = "fileOfPurchase.dat";

	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in | ios::out | ios::app);
	if (PurchaseFile.Write(p) == -1) {
		cout << "Insert Error!" << endl;
	}
	PurchaseFile.Close();

}

void updatePurchase_Customer(char* recID, char AccessID[]) {
	int num = purchaseRecordCount();
	ifstream ifs("fileOfPurchase.dat");
	string filename;
	char file_name[20] = "fileOfPurchase.dat";
	int index = 0;
	int up_index = 0;
	Purchase p;
	int ifs_last;
	int find_ID = 0;
	DelimFieldBuffer buffer('|', 1000);
	RecordFile <Purchase> PurchaseFile(buffer);
	Purchase* purchase_arr = new Purchase[100000];

	if (ifs.fail()) {
		cout << "fileOfPurchase.dat open error!" << endl;
		return;
	}
	ifs.ignore(numeric_limits<streamsize>::max(), '\n');

	PurchaseFile.Open(file_name, ios::in);
	ifs.clear();	//because of c++98
	ifs.seekg(0, ifs.end);
	ifs_last = ifs.tellg();
	ifs.seekg(0, ifs.beg);
	for (int i = 0; i < num; i++) {
		PurchaseFile.Read(p);
		//cout << p.getPurchaseID() << " error! " << PurchaseID << endl;
		if (strncmp(p.getPurchaseID(), recID, 12) == 0) {
			up_index = i;
			purchase_arr[i] = p;
			//ifs.seekg(p.get_length(), ifs.cur);
			//break;
		}
		else {
			purchase_arr[i] = p;
		}
		//ifs.seekg(p.get_length(), ifs.cur);
	}
	p = purchase_arr[up_index];
	string temp;
	char* update_temp = new char(100);

	cout << "++++++++++++++++++++++++" << endl;
	cout << "input new information" << endl;

	cout << "new Game ID(8char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 8) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_game_id(update_temp);

	p.update_member_id(AccessID);

	cout << "new Purchase Date(10char): ";
	cin >> update_temp;
	if (strlen(update_temp) != 10) {
		cout << "Wrong length" << endl;
		return;
	}
	p.update_purchase_date(update_temp);

	ifs.seekg(p.get_length(), ifs.cur);
	cout << endl;
	cout << "Update success!!!" << endl;
	cout << p;


	purchase_arr[up_index] = p;
	ifs.seekg(p.get_length(), ifs.cur);

	PurchaseFile.Close();

	PurchaseFile.Create(file_name, ios::out | ios::trunc);
	int recaddr;
	for (int i = 0; i < num; i++) {
		if ((recaddr = PurchaseFile.Write(purchase_arr[i])) == -1)
			cout << "Write Error!" << endl;
	}
	PurchaseFile.Close();

	ifs.close();
}

void deleteMemPurchase_Consumer(char* AccessID) {
	char p_id[13];
	int num, addr, find = -1;
	char file_name[20] = "fileOfPurchase.dat";
	char* ID;
	string memID;


	num = purchaseRecordCount();
	DelimFieldBuffer buffer('|', STDMAXBUF);
	RecordFile <Purchase> PurchaseFile(buffer);

	PurchaseFile.Open(file_name, ios::in);
	//cout << num << endl;

	for (int i = 0; i < num; i++) {
		Purchase p;
		addr = PurchaseFile.Read(p);
		memID = p.getMemberID();
		cout << memID << endl;
		if (!strcmp(AccessID, memID.c_str())) {
			//cout << p << endl;
			find = addr;
			strncpy(p_id, p.getPurchaseID(), 12);
			p_id[12] = '\0';

			deletePurchase(p_id);
			//cout << p_id << endl;
		}
	}

	
	PurchaseFile.Close();
}

char Purchase::Key() {
	return key;
}