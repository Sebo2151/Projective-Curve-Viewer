/*
    Projective Curve Viewer
    Copyright (C) 2016  Sebastian Bozlee

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef TERM_H
#define TERM_H

#include <qstring.h>
#include <QVector4D>

class Term
{
public:
    Term(){}
    virtual ~Term() {}

    // Warning: allocates a new Term.
    static Term* parseTerm(std::string input);
    virtual double eval(double x, double y, double z, double s, double t) = 0;
    double eval(QVector4D v, double s, double t) { return eval(v.x(), v.y(), v.z(), s, t); }
    int priority() { return my_priority; }
    void setPriority(int priority) { my_priority = priority; }

    // Warning: allocates a new Term.
    virtual Term* derivative(char var) = 0;
    virtual void print() = 0;
    virtual Term* Clone() = 0;

    // Warning: allocates a new Term.
    virtual Term* simplify() { return Clone(); }
    // Warning: allocates a new Term.
    virtual Term* homogenize(int* degree) = 0;

    virtual bool isZero() { return false; }
    virtual bool isOne() { return false; }
    virtual bool isNumerical() { return false; }
protected:
    int my_priority = 0;
};

class BadTermException
{
public:
    BadTermException(const char* error_message = 0) { message = error_message; }
    const char* getErrorMessage() { return message; }
private:
    const char* message = 0;
};

#endif // TERM_H
