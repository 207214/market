# include <iostream>
# include <list>
# include <time.h> //for dairy and alcohol classes
# include <cstring>
# include <unistd.h> // sleep() in 'main' to test dairy 

using namespace std;

// for "Dairy" class
# define DAY 86400
# define WEEK 604800
# define MONTH 2592000

/*
    Абстрактный класс - товар. Каждый из наследников имеет перегр
    operator()(), которая использует функции check_customer() и
    check_goods(); это чистые виртуальные функции;
    для одних наследников важна первая, для других - обе
    (например, alcohol, cigar);  
*/

class Goods {

protected:

    static int total;
    char *name;
    double price;
    
    void constr_helper(const char *s);

public:

    Goods(double val, const char *nm): price(val) { constr_helper(nm); total++; }
    
    Goods(const Goods& src): price(src.price) { constr_helper(src.name); total++; }
    
    Goods& operator=(const Goods& src) { price = src.price; constr_helper(src.name); return *this; }
    
    virtual ~Goods() { if (name) delete [] name; total--; }

    double get_price() const { return price; }
    
    const char * get_name() const { return name; }
    
    virtual int check_goods(int ind = 0) = 0;
    virtual int check_customer(int ind = 0) = 0;
    
    friend ostream& operator<<(ostream&,  const Goods&);
    
    void refresh(double per100) { if (name) price *= 1+(per100/100); if (price <= 0) price = 0; }
    
    int operator() (int ind1 = 0, int ind2 = 0)
    {
        if ( (!check_customer(ind1)) && (!check_goods(ind2)) )
            return 0;
        return 1;
    }
};

ostream& operator<<(ostream& outstream, const Goods& src)
{
    outstream << "Product: " << src.get_name() << endl << "Price: " << src.get_price() << endl; 
    return outstream;
}

void Goods::constr_helper(const char *s)
{
    size_t name_len = strlen(s);
    name = new char[name_len+1];
    for (unsigned i = 0; i <= name_len; ++i)
        name[i] = s[i];
}


class Dairy : public Goods {

    tm * begin;
    tm *end;
    unsigned shelf_life;
    bool valid;
    int num;
    
    void dairy_helper(unsigned sec);
    
public:

    Dairy(double val, const char *nm, unsigned sec, int n = 1): Goods(val, nm), shelf_life(sec), valid(0)
    {
        num = n;
        dairy_helper(sec);
    }
    
    Dairy(const Dairy& src): Goods(src.price, src.name), shelf_life(src.shelf_life), valid(0)
    {
        dairy_helper(src.shelf_life);
    }
 

    void get_begin() { cout << asctime(begin); }
    void get_end() { cout << asctime(end); }
    int get_num() const { return num; }
    virtual ~Dairy() { if (end) delete end; if (begin) delete begin; }
    void delete_helper() {
        if (name) { delete [] name; name = 0; }
        if (end) { delete end; end = 0; }
        if (begin) { delete begin; begin = 0; } //
        cout << "Product deleted from the system\n";
    }
    
    int check_goods(int ind = 0)
    {
        time_t t = time(0);
        if (t > mktime(end))
        {
            valid = 1;
            cout << "Dairy product "  << '<' << name << '>'  << " is expired\n";
            cout << "Press 'y' to inform  the worker\n";
            if (cin.get() == 'y')
            {
                delete_helper();
            }
            return 1;
        }
        else if (num <= 0) return 1;
        num--;
        return 0;
    }
    
    int check_customer(int ind = 0) { return 0; } //0+
};

void Dairy::dairy_helper(unsigned sec)
{
    time_t t = time(0);
    tm *tmp = localtime(&t);
    begin = new tm;
    end = new tm;
    *begin = *tmp;
    *end = *begin;
    end->tm_sec += sec;
    mktime(end);
}



class Alcohol : public Goods {

    int limit;
    int num;
    int hour_limit;
    
public:

    Alcohol(double val, const char *nm, int lim = 0, int n = 1, int hlim = 0): Goods(val, nm), limit(lim), num(n), hour_limit(hlim) {}
    
    int check_customer(int ind = 0)
    {
        if (ind < limit)
        {
            cout << "Sorry, but  " << '<' << name << '>' << " can`t be sold! You`re too young\n";
            return 1;
        }
        return 0;
    }
    int check_goods(int ind = 0)
    {
        time_t t = time(0);
        tm *tmp = localtime(&t);
        if (tmp->tm_hour >= hour_limit) { cout << "Too late! Welcome you tomorrow\n"; return 1; }
        if (num <= 0) { cout << name << " ended\n"; return 1; }
        num--;
        return 0;
    }
    
    int get_limit() const { return limit; }
};

class Cigar : public Goods {

    int limit;
    int num;

public:

    Cigar(double val, const char *nm, int lim = 0, int n = 1): Goods(val, nm), limit(lim), num(n) {}
    
    int check_goods(int ind = 0)
    {
        if (num <= 0) { cout << name << " ended\n"; return 1; }
        num--;
        return 0;   
    }
    
    int check_customer(int ind = 0)
    {
        if (ind < limit)
        {
            cout << "Sorry, but  " << '<' << name << '>' << " can`t be sold! You`re too young\n";
            return 1;
        }
        return 0; 
    }
};
    
class Grocery : public Goods {

    double total_weight;

public:

    Grocery(double val, const char *nm, double kg): Goods(val, nm), total_weight(kg) {}
    int check_goods(int ind = 0)
    {
        if (ind <= total_weight) { total_weight -= ind; return 0;}
        cout << "Too much kg! ";
        return 1;
    }
    
    int check_customer(int ind = 0) { return 0; } //0+
    
    int get_total_weight() const { return total_weight; }
};

int Goods::total = 0;


int main()
{
    Dairy a(95, "milk", 2);
    Grocery b(97, "apple", 300);
    Alcohol c(96, "wine", 21, 10);
    Cigar d(94, "parl", 53, 10);
    int tmp = d(53, 2);
    cout << tmp << endl;
    cout << d << c << b << a;
//    sleep(1);
    a();
    b(20, 310);
   /* a.get_begin();
    a.get_end();
    sleep(2);
    Dairy b(a);
    b.get_begin();
    b.get_end();*/
    return 0;
}
