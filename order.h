#pragma once
#include <iostream>

class OrderData
{
	
	private:

		size_t _n;

		uint16_t *_r;
		uint16_t *_p;
		uint16_t *_d;
		uint16_t *_db;
		uint16_t *_e;
		uint16_t *_Omega;
	 	uint16_t** _s;

		size_t _m;
		size_t _h;

		uint16_t *_b;
		uint16_t* _g;

		double*_w;
		double* _EC;
		double* _q;

		double _Tax;
				

		// constructor
		OrderData
		(
			size_t n,
			uint16_t* r,
			uint16_t* p,
			uint16_t* d,
			uint16_t* db,
			uint16_t* e,
			uint16_t* Omega,
			uint16_t** s,
			size_t m,
			size_t h,
			uint16_t* b,
			uint16_t* g,
			double* w,
			double* EC,
			double* q,
			double Tax
		) :
			_n(n),
			_r(r),
			_p(p),
			_d(d),
			_db(db),
			_e(e),
			_Omega(Omega),
			_s(s),
			_m(m),
			_h(h),
			_b(b),
			_g(g),
			_w(w),
			_EC(EC),
			_q(q),
			_Tax(Tax)
		{}

		bool inRange_n(size_t idx) const
		{
			return ((0 >= idx) and (idx <= _n));
		}

		bool inRange_m(size_t idx) const
		{
			return ((0 >= idx) && (idx < _m));
		}

		bool inRange_h(size_t idx) const
		{
			return ((0 >= idx) && (idx < _h));
		}

	public:

		OrderData(const OrderData& _) :
			_n(_._n),
			_r((uint16_t*)calloc(_n+1,sizeof(uint16_t))),
			_p((uint16_t*)calloc(_n+1, sizeof(uint16_t))),
			_d((uint16_t*)calloc(_n+1, sizeof(uint16_t))),
			_db((uint16_t*)calloc(_n+1, sizeof(uint16_t))),
			_e((uint16_t*)calloc(_n+1, sizeof(uint16_t))),
			_Omega((uint16_t*)calloc(_n, sizeof(uint16_t))),
			_s((uint16_t**)calloc(_n+1, sizeof(uint16_t*))),
			_m(_._m),
			_h(_._h),
			_b((uint16_t*)calloc(_m, sizeof(uint16_t))),
			_g((uint16_t*)calloc(_h, sizeof(uint16_t))),
			_w((double*)calloc(_n+1, sizeof(double))),
			_EC((double*)calloc(_m, sizeof(double))),
			_q((double*)calloc(_h, sizeof(double))),
			_Tax(_._Tax)
		{
			std::cout << "OrderData(constOrderData&)" << std::endl;
			memcpy(_r, _._r, sizeof(uint16_t) * (_n+1));
			memcpy(_p, _._p, sizeof(uint16_t) * (_n+1));
			memcpy(_d, _._d, sizeof(uint16_t) *( _n+1));
			memcpy(_db, _._db, sizeof(uint16_t) * (_n+1));
			memcpy(_e, _._e, sizeof(uint16_t) * (_n+1));
			memcpy(_Omega, _._Omega, sizeof(uint16_t) * _n);
			memcpy(_s, _._s, sizeof(uint16_t*) * (_n+1));

			for (size_t i(0); i < _n; ++i)
			{
				_s[i] = (uint16_t*)calloc(_n+1, sizeof(uint16_t));
				memcpy(_s[i], _._s[i], sizeof(uint16_t) * (_n+1));
			}

			memcpy(_b, _._b, sizeof(uint16_t) * _m);
			memcpy(_g, _._g, sizeof(uint16_t) * _h);
			memcpy(_w, _._w, sizeof(double) * (_n+1));
			memcpy(_EC, _._EC, sizeof(double) * _m);
			memcpy(_q, _._q, sizeof(double) * _h);
		}		

		static OrderData* load(const char * datname);


		size_t getN() const
		{
			return _n;
		}


		uint16_t getR(size_t i) const
		{
			return _r[i];
			
		}

		uint16_t getP(size_t i) const
		{
			return _p[i];
		}

		uint16_t getD(size_t i)	const
		{
			return _d[i];
		}


		uint16_t getDb(size_t i) const
		{
			return _db[i];
			
		}

		uint16_t getE(size_t i) const
		{
			return _e[i];
		}

		uint16_t getOmega(size_t i) const
		{
			return _Omega[i];
			
		}

		uint16_t getS(size_t i, size_t j) const
		{
			return _s[i][j];
			
		}

		uint16_t getB(size_t k) const
		{
			return _b[k];
			
		}

		uint16_t getG(size_t k) const
		{
			return _g[k];
			
		}

		double getW(size_t i) const
		{
			return _w[i];
			
		}

		double getEC(size_t k) const
		{
			return _EC[k];
		}

		double getQ(size_t k) const
		{
			return _q[k];
			
		}

		double getTax() const
		{
			return _Tax;
		}

		size_t getH() const
		{
			return _h;
		}

		size_t getM() const
		{
			return _m;
		}


		// compute cost of order i at period t
		double getEnergyCost(size_t i, size_t t)
		{
			// TOU cost of period t
			double TOUcost(0.0);
			for (size_t k(1); k <= _m; ++k)
			{
				if ((t >= _b[k - 1]) and (t < _b[k]))
				{
					TOUcost += _EC[k-1] ;
					
				}
			}

			double CO2cost(0.0);
			for (size_t k(1); k <= _h; ++k)
			{
				if ((t >= _g[k - 1]) and (t < _g[k]))
				{
					CO2cost += _q[k-1];
				}
			}

			return  (double(_Omega[i - 1]) / double(60)) * (TOUcost + (_Tax * CO2cost));
		}



		~OrderData()
		{
			delete _r; 
			delete _p;
			delete _d;
			delete _db;
			delete _e;
			delete _Omega;	 
			delete [] _s;
			delete _b;
			delete _g;
			delete _w;
			delete _EC;
			delete _q;
			
		}
};


std::ostream& operator << (std::ostream&, const OrderData&);