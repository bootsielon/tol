/* tol_bctmsbas.h - CTimeSet Tol Data Type creation.
 *                  Basic Datings implementation: Y, M, D, H, Mi, S, W, WD
 *
 * Copyright (C) 2005 - Bayes Decision, SL
 * author: Daniel Rus <danirus@tol-project.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */

#ifndef TOL_BCTMSBAS_H
#define TOL_BCTMSBAS_H 1

#include <tol/tol_bctime.h>
#include <tol/tol_cbtol.h>
#include <tol/tol_bctimeset.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctmigra.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
/*! This CTimeSet represents the whole time into a given year.
 */
class BCTmsYear : public BCTmsTemporary
{
 private:
    BCTime year_;
 public:
    BCTmsYear(BList *arg) : BCTmsTemporary(arg) 
	{
	    year_ = BCTime(static_cast<long int>(Real(Arg(1))));
	    setGranul(YEARS);
	    setType(CTmsYear);
	}

    BCTmsYear(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);

	    long int year = 0L;
	    fread(&year, 1, sizeof(long int), fil);
	    year_ = BCTime(year);

	    setGranul(YEARS);
	    setType(CTmsYear);
    	}
    
  ~BCTmsYear() { }

    void* getAttr() { return &year_; }
 
    bool includes(const BCTime& tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime& tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime& tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsYear);

//--------------------------------------------------------------------
/*! This CTimeSet represents the whole time into a given month.
 */
class BCTmsMonth : public BCTmsTemporary
{
 private:
    int month_;
 public:
    BCTmsMonth(BList *arg) : BCTmsTemporary(arg) 
	{
	    month_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((month_<1) || (month_>12))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a month: " << month_ << endl;
		EN_ss << "Must be between 1 and 12, both included";
		ES_ss << "Valor erróneo para el mes: " << month_ << endl;
		ES_ss << "Debe estar entre 1 y 12, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(MONTHS);
	    setType(CTmsMonth);
	}
    
    BCTmsMonth(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);
    fread(&month_, 1, sizeof(int), fil);
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((month_<1) || (month_>12))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a month: " << month_ << endl;
		EN_ss << "Must be between 1 and 12, both included";
		ES_ss << "Valor erróneo para el mes: " << month_ << endl;
		ES_ss << "Debe estar entre 1 y 12, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(MONTHS);
	    setType(CTmsMonth);
    	}
    
  ~BCTmsMonth() {}

    void* getAttr() { return &month_; }
    int getHLimit() const { return 12; }

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);    

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsMonth);

//--------------------------------------------------------------------
/*! This CTimeSet represents the whole time into a given day of a month.
 */
class BCTmsDay : public BCTmsTemporary
{
 private:
    int day_;
 public:
    BCTmsDay(BList *arg) : BCTmsTemporary(arg) 
	{
	    day_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((day_<-31) || (day_==0) || (day_>31))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a day: " << day_ << endl;
		EN_ss << "Must be between 1 and 31, both included";
		ES_ss << "Valor erróneo para el día: " << day_ << endl;
		ES_ss << "Debe estar entre 1 y 31, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(DAYS);
	    setType(CTmsDay);
	}
    
    BCTmsDay(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);

	    fread(&day_, 1, sizeof(int), fil);
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((day_<-31) || (day_==0) || (day_>31))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a day: " << day_ << endl;
		EN_ss << "Must be between 1 and 31, both included";
		ES_ss << "Valor erróneo para el día: " << day_ << endl;
		ES_ss << "Debe estar entre 1 y 31, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(DAYS);
	    setType(CTmsDay);
    	}

  ~BCTmsDay() {}
    
    void* getAttr() { return &day_; }
    int getHLimit() const { return 31; }

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);    

    BCTime pred(const BCTime &tmi);
//    BCTime pred4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsDay);

//--------------------------------------------------------------------
/*! This CTimeSet represents the whole time into a given hour of a day.
 */
class BCTmsHour : public BCTmsTemporary
{
 private:
    int hour_;
 public:
    BCTmsHour(BList *arg) : BCTmsTemporary(arg) 
	{
	    hour_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((hour_<0) || (hour_>23))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for an hour: " << hour_ << endl;
		EN_ss << "Must be between 0 and 23, both included";
		ES_ss << "Valor erróneo para la hora: " << hour_ << endl;
		ES_ss << "Debe estar entre 0 y 23, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(HOURS);
	    setType(CTmsHour);
	}
    
    BCTmsHour(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);	    
    fread(&hour_, 1, sizeof(int), fil);
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((hour_<0) || (hour_>23))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for an hour: " << hour_ << endl;
		EN_ss << "Must be between 0 and 23, both included";
		ES_ss << "Valor erróneo para la hora: " << hour_ << endl;
		ES_ss << "Debe estar entre 0 y 23, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(HOURS);
	    setType(CTmsHour);
    	}
    
    ~BCTmsHour() { }

    void* getAttr() { return &hour_; }
    int getHLimit() const { return 24; }
    
    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);    

    BCTime pred(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsHour);

//--------------------------------------------------------------------
/*! This CTimeSet represents the whole time into a given minute of an hour.
 */
class BCTmsMinute : public BCTmsTemporary
{
 private:
    int minute_;
 public:
    BCTmsMinute(BList *arg) : BCTmsTemporary(arg) 
	{
	    minute_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((minute_<0) || (minute_>59))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a minute: " << minute_ << endl;
		EN_ss << "Must be between 0 and 59, both included";
		ES_ss << "Valor erróneo para el minuto: " << minute_ << endl;
		ES_ss << "Debe estar entre 0 y 59, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(MINUTES);
	    setType(CTmsMinute);
	}
    
    BCTmsMinute(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);	    
	    
	    fread(&minute_, 1, sizeof(int), fil);
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((minute_<0) || (minute_>59))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a minute: " << minute_ << endl;
		EN_ss << "Must be between 0 and 59, both included";
		ES_ss << "Valor erróneo para el minuto: " << minute_ << endl;
		ES_ss << "Debe estar entre 0 y 59, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(MINUTES);
	    setType(CTmsMinute);
    	}

  ~BCTmsMinute() {}

    
    void* getAttr() { return &minute_; }
    int getHLimit() const { return 59; }

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);    

    BCTime pred(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsMinute);

//--------------------------------------------------------------------
/*! This CTimeSet represents a given second of a minute.
 */
class BCTmsSecond : public BCTmsTemporary
{
 private:
    int second_;
 public:
    BCTmsSecond(BList *arg) : BCTmsTemporary(arg) 
	{
	    second_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((second_<0) || (second_>59))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a second: " << second_ << endl;
		EN_ss << "Must be between 0 and 59, both included";
		ES_ss << "Valor erróneo para el segundo: " << second_ << endl;
		ES_ss << "Debe estar entre 0 y 59, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(SECONDS);
	    setType(CTmsSecond);
	}
    
    BCTmsSecond(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);
	    fread(&second_, 1, sizeof(int), fil);

	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((second_<0) || (second_>59))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for a second: " << second_ << endl;
		EN_ss << "Must be between 0 and 59, both included";
		ES_ss << "Valor erróneo para el segundo: " << second_ << endl;
		ES_ss << "Debe estar entre 0 y 59, ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(SECONDS);
	    setType(CTmsSecond);
	}
    
  ~BCTmsSecond() {}
    
    void* getAttr() { return &second_; }
    int getHLimit() const { return 59; }

    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsSecond);

//--------------------------------------------------------------------
/*! This CTimeSet represents a given second of a minute.
 */
class BCTmsWeekDay : public BCTmsTemporary
{
 private:
    int wd_;
 public:
    BCTmsWeekDay(BList *arg) : BCTmsTemporary(arg) 
	{
	    wd_ = static_cast<int>(Real(Arg(1)));
	    
	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((wd_<1) || (wd_>7))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for the week day: " << wd_ << endl
		      << "Must be between 1 and 7 (Monday to Sunday), "
		      << "both included";
		ES_ss << "Valor erróneo para el día de la semana: " << wd_ 
		      << endl
		      << "Debe estar entre 1 y 7 (Lunes a Domingo), "
		      << "ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(DAYS);
	    setType(CTmsWeekDay);
	}
    
    BCTmsWeekDay(const BText& name, const BText& description, FILE* fil) 
	: BCTmsTemporary() 
	{
    PutName(name);
    PutDescription(description);
    IncNRefs();
    Grammar()->AddObject(this);
	    fread(&wd_, 1, sizeof(int), fil);

	    // ¿Out of bounds? -> throw an exception; evaluator will catch it
	    if((wd_<1) || (wd_>7))
	    {
		std::stringstream EN_ss, ES_ss;
		EN_ss << "Wrong value for the week day: " << wd_ << endl
		      << "Must be between 1 and 7 (Monday to Sunday), "
		      << "both included";
		ES_ss << "Valor erróneo para el día de la semana: " << wd_ 
		      << endl
		      << "Debe estar entre 1 y 7 (Lunes a Domingo), "
		      << "ambos incluídos";
		throw BException(EN_ss.str(), ES_ss.str());
	    }
	    
	    setGranul(DAYS);
	    setType(CTmsWeekDay);
	}

    ~BCTmsWeekDay() {}

    void* getAttr() { return &wd_; }
    int getHLimit() const { return 7; }
    
    bool includes(const BCTime &tmi);
//    bool includes4Succ(const BCTime &tmi, BCTimeSet *uctms);
    bool includes4SuccInG(const BCTime &tmi, int niter, granularity granul);
    bool includes4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime succ(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime succ4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime succ4SelfSucc(const BCTime &tmi, int ndisp);

    BCTime pred(const BCTime &tmi);
//    BCTime succ4Succ(const BCTime &tmi, BCTimeSet *uctms);
    BCTime pred4SuccInG(const BCTime &tmi, int niter, granularity granul);
    BCTime pred4SelfSucc(const BCTime &tmi, int ndisp);    

    bool isVoidIntersectedWith(BCTimeSet *ctms) const;
    bool isSuccableInG(int niter, granularity granul) const;
    int BinWrite(FILE* fil) const;
};

DeclareCTimeSetEvaluator(BCTmsWeekDay);

#endif /* __USE_TC__ */
#endif /* TOL_BCTMSBAS_H */
