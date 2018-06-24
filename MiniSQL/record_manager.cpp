#include"database.h"
#include"table.h"
#include"column.h"
#include"record_manager.h"
#include"catalog_manager.h"
#include"real_buffer_manager.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<string.h>


using namespace std;

char table1[][2][10] = {
	"001","name1",
	"002","name2",
	"003","name3",
	"004","name4"
};


char ***initData() {

	char table2[][4][10] = {
		"001","andy","computer","4.0",
		"002","peter","biology","3.1",
		"003","sam","chemistry","4.4"
	};
	char ***data;
	int row_num = 3;
	int column_num = 4;
	data = new char**[row_num + 1];
	for (int i = 0; i < row_num; i++) {
		data[i] = new char*[column_num];
		for (int j = 0; j < column_num; j++) {
			data[i][j] = new char[10];
			strcpy(data[i][j], table2[i][j]);
		}
	}
	return data;
}

table* record_manager::select(m_string tableName, m_string *columns, int columnNum, m_string column, m_string value, char opt) {

	database *db = this->dict.db;
	table *tb = new table();
	real_buffer_manager r;
	int rs = 0;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i].table_name) {
			*tb = db->tables[i];
			rs = 1;
			break;
		}
	}
	//�����������
	//if (rs == 0) throw error;

	//����buffer manager������������ڴ��Ϊchar [][][]��ʽ
	//char ***data = buffer_manager.read(tableName);

	m_string ** data = r.read_table(tb->table_name, tb->row_num, tb->column_num);
	int t = 0;
	m_string **newData;
	//���Լ������
	if (opt != ' ') {
		int r = 0, c = 0, isInt = 0, isFloat = 0;;
		for (int i = 0; i < tb->column_num; i++) {
			if (column == tb->columns[i].column_name) {
				c = i;
				if (strcmp(tb->columns[i].data_type.str, "int") == 0)
					isInt = 1;
				if (strcmp(tb->columns[i].data_type.str, "float") == 0)
					isFloat = 1;
				break;
			}
		}
		newData = new m_string*[tb->row_num];
		for (int i = 0; i < tb->row_num; i++) {
			newData[i] = new m_string[tb->column_num];
			string s;
			stringstream ss;
			int n, v;
			float f;
			switch (opt) {
			case '=':
				if (isInt || isFloat) {
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n == v) {
						newData[r++] = data[i];
					}
					else if (isFloat&&f == v) {
						newData[r++] = data[i];
					}
				}
				else {
					if (strcmp(value.str, data[i][c].str) == 0)
						newData[r++] = data[i];
				}
				break;
			case'>':
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;

				if (isInt&&n > v) {
					newData[r++] = data[i];
				}
				else if (isFloat&&f > v) {
					newData[r++] = data[i];
				}
				break;
			case'<':
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;
				if (isInt&&n < v) {
					newData[r++] = data[i];
				}
				else if (isFloat&&f < v) {
					newData[r++] = data[i];
				}
				break;
			case'g':
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;
				if (isInt&&n >= v) {
					newData[r++] = data[i];
				}
				else if (isFloat&&f >= v) {
					newData[r++] = data[i];
				}
				break;
			case'l':
				s = data[i][c].str;
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;
				if (isInt&&n <= v) {
					newData[r++] = data[i];
				}
				else if (isFloat&&f <= v) {
					newData[r++] = data[i];
				}
				break;
			case'!':
				if (isInt || isFloat) {
					int n, v;
					s = data[i][c].str;
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value;
					ss >> v;
					if (isInt&&n != v) {
						newData[r++] = data[i];
					}
					else if (isFloat&&f != v) {
						newData[r++] = data[i];
					}
				}
				else {
					if (strcmp(value.str, data[i][c].str) != 0)
						newData[r++] = data[i];
				}
				break;
			}
		}
		tb->row_num = r;
	}
	else {
		newData = data;
	}

	// select *
	if (columnNum == 1 && strcmp(columns[0].str, "*") == 0) {
		for (int i = 0; i < tb->row_num; i++) {
			tb->rows[i] = new row(newData[i], tb->column_num);
		}
		return tb;
	}
	// select id name
	else {
		int colId[10];
		int t = 0;
		for (int i = 0; i < columnNum; i++) {
			for (int j = 0; j < tb->column_num; j++) {
				if (columns[i] == tb->columns[j].column_name) {
					colId[t++] = j;
					break;
				}
			}
		}

		for (int i = 0; i < tb->row_num; i++) {
			tb->rows[i] = new row(newData[i], tb->column_num, colId);
		}
		tb->column_num = columnNum;
		return tb;
	}

}

int record_manager::add(m_string tableName, m_string* newRow)
{
	database *db = this->dict.db;
	table *tb = new table();
	real_buffer_manager r;
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i].table_name) {
			*tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	m_string **originData = r.read_table(tb->table_name, tb->row_num, tb->column_num);
	originData[tb->row_num] = new m_string[tb->column_num];
	//if(rs==0)throw error
	int rnum = tb->row_num;
	for (int i = 0; i < tb->column_num; i++) {
		strcpy(originData[rnum][i].str, newRow[i].str);
	}


	//����buffer manager�������ĺ�����ݴ���Ӳ��
	int result = r.update_table(tb->table_name, originData, tb->row_num + 1, tb->column_num);


	//���������ֵ������
	this->dict.db->tables[t].row_num++;
	this->dict.update_database();
	return 0;
}

int record_manager::_delete(m_string tableName, m_string column, m_string value, char opt)
{
	database *db = this->dict.db;
	table *tb = new table();
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i].table_name) {
			*tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	// if(rs==0) throw error;


	char ***data = initData();

	/*
		���Լ����
	*/
	int r = 0, c = 0, isInt = 0, isFloat = 0;
	for (int i = 0; i < tb->column_num; i++) {
		if (column == tb->columns[i].column_name) {
			c = i;
			if (strcmp(tb->columns[i].data_type.str, "int") == 0)
				isInt = 1;
			if (strcmp(tb->columns[i].data_type.str, "float") == 0)
				isFloat = 1;
			break;
		}
	}
	//��������
	for (int i = 0; i < tb->row_num; i++) {
		string s;
		stringstream ss;
		int n, v;
		float f;
		switch (opt) {
		case '=':
			if (isInt || isFloat) {
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;
				if (isInt&&n == v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
				else if (isFloat&&f == v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
			}
			else {
				if (strcmp(value.str, data[i][c]) == 0) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
			}
			break;
		case'>':
			s = data[i][c];
			ss << s;
			if (isInt == 1)
				ss >> n;
			if (isFloat == 1)
				ss >> f;
			ss.clear();
			ss << value;
			ss >> v;
			if (isInt&&n > v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			else if (isFloat&&f > v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			break;
		case'<':
			s = data[i][c];
			ss << s;
			if (isInt == 1)
				ss >> n;
			if (isFloat == 1)
				ss >> f;
			ss.clear();
			ss << value;
			ss >> v;
			if (isInt&&n < v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			else if (isFloat&&f < v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			break;
		case'g':
			s = data[i][c];
			ss << s;
			if (isInt == 1)
				ss >> n;
			if (isFloat == 1)
				ss >> f;
			ss.clear();
			ss << value;
			ss >> v;
			if (isInt&&n >= v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			else if (isFloat&&f >= v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			break;
		case'l':
			s = data[i][c];
			ss << s;
			if (isInt == 1)
				ss >> n;
			if (isFloat == 1)
				ss >> f;
			ss.clear();
			ss << value;
			ss >> v;
			if (isInt&&n <= v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			else if (isFloat&&f <= v) {
				tb->row_num--;
				for (int j = i; j < tb->row_num; j++) {
					data[j] = data[j + 1];
				}
			}
			break;
		case'!':
			if (isInt || isFloat) {
				int n, v;
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value;
				ss >> v;
				if (isInt&&n != v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
				else if (isFloat&&f != v) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
			}
			else {
				if (strcmp(value.str, data[i][c]) != 0) {
					tb->row_num--;
					for (int j = i; j < tb->row_num; j++) {
						data[j] = data[j + 1];
					}
				}
			}
			break;
		}
	}

	/*
	data���µ��ļ�
	�������ݴʵ�
	*/
	this->dict.db->tables[t].row_num = tb->row_num;
	this->dict.update_database();

	//����buffer manager�������ĺ�����ݴ���Ӳ��
	//��table���±��char newData[][][]����
	//int result = buffer_manager.update(tableName,newData);

	return 0;
}

int record_manager::update(m_string tableName, m_string column1, m_string value1, m_string column2, m_string value2, char opt)
{
	database *db = this->dict.db;
	table *tb = new table();
	int rs = 0;
	int t;
	for (int i = 0; i < db->tableNum; i++) {
		if (tableName == db->tables[i].table_name) {
			*tb = db->tables[i];
			t = i;
			rs = 1;
			break;
		}
	}

	// if(rs==0) throw error;

	char ***data = initData();
	/*
	��������
	*/
	int t1 = 0;
	for (int i = 0; i < tb->column_num; i++) {
		if (column1 == tb->columns[i].column_name) {
			t1 = i;
			break;
		}
	}
	if (opt == ' ') {
		for (int i = 0; i < tb->row_num; i++) {
			strcpy(data[i][t1], value1.str);
		}
	}
	else {
		/*
		���Լ����
		*/
		int r = 0, c = 0, isInt = 0, isFloat = 0;
		for (int i = 0; i < tb->column_num; i++) {
			if (column2 == tb->columns[i].column_name) {
				c = i;
				if (strcmp(tb->columns[i].data_type.str, "int") == 0)
					isInt = 1;
				if (strcmp(tb->columns[i].data_type.str, "float") == 0)
					isFloat = 1;
				break;
			}
		}
		//��������
		for (int i = 0; i < tb->row_num; i++) {
			string s;
			stringstream ss;
			int n, v;
			float f;
			switch (opt) {
			case '=':
				if (isInt || isFloat) {
					s = data[i][c];
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value2;
					ss >> v;
					if (isInt&&n == v) {
						strcpy(data[i][t1], value1.str);
					}
					else if (isFloat&&f == v) {
						strcpy(data[i][t1], value1.str);
					}
				}
				else {
					if (strcmp(value2.str, data[i][c]) == 0) {
						strcpy(data[i][t1], value1.str);
					}
				}
				break;
			case'>':
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value2;
				ss >> v;
				if (isInt&&n > v) {
					strcpy(data[i][t1], value1.str);
				}
				else if (isFloat&&f > v) {
					strcpy(data[i][t1], value1.str);
				}
				break;
			case'<':
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value2;
				ss >> v;
				if (isInt&&n < v) {
					strcpy(data[i][t1], value1.str);
				}
				else if (isFloat&&f < v) {
					strcpy(data[i][t1], value1.str);
				}
				break;
			case'g':
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value2;
				ss >> v;
				if (isInt&&n >= v) {
					strcpy(data[i][t1], value1.str);
				}
				else if (isFloat&&f >= v) {
					strcpy(data[i][t1], value1.str);
				}
				break;
			case'l':
				s = data[i][c];
				ss << s;
				if (isInt == 1)
					ss >> n;
				if (isFloat == 1)
					ss >> f;
				ss.clear();
				ss << value2;
				ss >> v;
				if (isInt&&n <= v) {
					strcpy(data[i][t1], value1.str);
				}
				else if (isFloat&&f <= v) {
					strcpy(data[i][t1], value1.str);
				}
				break;
			case'!':
				if (isInt || isFloat) {
					int n, v;
					s = data[i][c];
					ss << s;
					if (isInt == 1)
						ss >> n;
					if (isFloat == 1)
						ss >> f;
					ss.clear();
					ss << value2;
					ss >> v;
					if (isInt&&n != v) {
						strcpy(data[i][t1], value1.str);
					}
					else if (isFloat&&f != v) {
						strcpy(data[i][t1], value1.str);
					}
				}
				else {
					if (strcmp(value2.str, data[i][c]) != 0) {
						strcpy(data[i][t1], value1.str);
					}
				}
				break;
			}
		}
	}



	/*
	data���µ��ļ�
	�������ݴʵ�*/
	m_string **d = new m_string*[tb->row_num];
	for (int i = 0; i < tb->row_num; i++) {
		d[i] = new m_string[tb->column_num];
		for (int j = 0; j < tb->column_num; j++) {
			strcpy(d[i][j].str, data[i][j]);
		}
	}
	real_buffer_manager r;
	r.update_table(tb->table_name, d, tb->row_num, tb->column_num);
	return 0;
}





record_manager::record_manager()
{
}

int main() {
	real_buffer_manager b;
	m_string str("table2");
	//b.create_dbFile(str);
	m_string s1("0004");
	m_string s2("cindy");
	m_string s3("computer science");
	m_string s4("4.0");
	m_string tbName("table2");
	m_string *s = new m_string[4]{ s1,s2,s3,s4 };
	record_manager rm;
	rm.add(tbName, s);
	m_string tableName;
	strcpy(tableName.str, "table2");
	table *tb = rm.select(tableName);
	for (int i = 0; i < tb->row_num; i++) {
		for (int j = 0; j < tb->column_num; j++) {
			cout << tb->rows[i]->data[j] << " ";
		}
		cout << endl;
	}
	system("pause");
}
