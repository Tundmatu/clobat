#ifndef __CVAR_H__
#define __CVAR_H__

#include <unordered_map>
#include <cassert>

#define BIT(x) (1 << x)

enum CVarType {
  CVAR_INT = 0,
  CVAR_FLOAT,
  CVAR_BOOL,
  CVAR_STRING
};

enum CVarFlag {
  CVAR_INT_T = BIT(0),
  CVAR_FLOAT_T = BIT(1),
  CVAR_BOOL_T = BIT(2),
  CVAR_STRING_T = BIT(3),
  CVAR_CONST = BIT(4),
  CVAR_READ = BIT(5),
  CVAR_WRITE = BIT(6),
};

class CVar {
public:
  CVar(int value, CVarFlag flags) :   m_ival(value), m_type(CVAR_INT),    m_flags(flags) {};
  CVar(float value, CVarFlag flags) : m_fval(value), m_type(CVAR_FLOAT),  m_flags(flags) {};
  CVar(bool value, CVarFlag flags) :  m_bval(value), m_type(CVAR_BOOL),   m_flags(flags) {};
  CVar(char *value, CVarFlag flags) : m_sval(value), m_type(CVAR_STRING), m_flags(flags) {};
  virtual ~CVar() {};

  void setInteger(int value)  { m_ival = value; m_type = CVAR_INT; }
  void setFloat(float value)  { m_fval = value; m_type = CVAR_FLOAT; }
  void setBool(bool value)    { m_bval = value; m_type = CVAR_BOOL; }
  void setString(char *value) { m_sval = value; m_type = CVAR_STRING; }

  int* const getInteger()  { assert(isInteger() && "CVar must have a int value");    return &m_ival; }
  float* const getFloat()  { assert(isFloat()   && "CVar must have a float value");  return &m_fval; }
  bool* const getBool()    { assert(isBool()    && "CVar must have a bool value");   return &m_bval; }
  char** const getString() { assert(isString()  && "CVar must have a string value"); return &m_sval; }

  CVarType getType() { return m_type; }

  bool isInteger() { return m_type == CVAR_INT; }
  bool isFloat()   { return m_type == CVAR_FLOAT; }
  bool isBool()    { return m_type == CVAR_BOOL; }
  bool isString()  { return m_type == CVAR_STRING; }

  static unsigned constexpr hash(const char *string) {
    return *string ? static_cast<unsigned int>(*string) + 33 * hash(string + 1) : 5381;
  }
private:
  CVarType m_type;
  CVarFlag m_flags;

  union {
    int m_ival;
    float m_fval;
    bool m_bval;
    char *m_sval;
  };

};

typedef std::shared_ptr<CVar> CVarShared;

typedef std::unordered_map<int, CVarShared> CVarMap;

class CVarSystem {
public:
  CVarSystem() {}
  virtual ~CVarSystem() {
    m_cvars.clear();
  }

  void put(int hash, CVarShared cvar) {
    if (has(hash)) return;

    m_cvars.insert(std::pair<int, CVarShared>(hash, cvar));
  }

  bool has(int hash) {
    return m_cvars.count(hash) > 0;
  }

  bool remove(int hash) {
    CVarMap::iterator it = m_cvars.find(hash);
    if (it != m_cvars.end()) {
      m_cvars.erase(it);

      return true;
    } else {
      return false;
    }
  }

  CVarShared get(int hash) {
    return has(hash) ? m_cvars.at(hash) : nullptr;
  }
private:
  CVarMap m_cvars;

};

#endif /* __CVAR_H__ */
