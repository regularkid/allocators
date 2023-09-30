// -----------------------------------------------------------------------------
class Thing
{
public:
    Thing()
    {
        printf("Thing Constructor\n");
    }

    ~Thing()
    {
        printf("Thing Destructor\n");
    }

public:
    char m_c = 0;
    short m_s = 0;
    int m_i = 0;
    long long m_l = 0;
    void* m_p = nullptr;
};