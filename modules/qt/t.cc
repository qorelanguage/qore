#include <QObject>

class Counter : public QObject
{
     Q_OBJECT

   public:
     Counter() { m_value = 0; }

      int value() const { return m_value; }

public slots:
 void setValue(int value);

      signals:
void valueChanged(int newValue);
      void valueChanged2(int newValue);

   private:
      int m_value;
};


void Counter::setValue(int value)
{
   if (value != m_value) {
      m_value = value;
      emit valueChanged(value);
   }
}
