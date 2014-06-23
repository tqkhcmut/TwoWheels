#ifndef abfilter_h
#define abfilter_h


class AlphaBetaFilter
{
	private:
		double alpha, beta, xk_1, vk_1;
		double xk, vk, rk;
	public:
		AlphaBetaFilter()
	{
		alpha = 0;
		beta = 0;
		xk_1 = 0;
		vk_1 = 0;
		xk = 0;
		vk = 0;
		rk = 0;
	}
	void InitializeAlphaBeta(double x_measured, double a, double b) 
	{
		alpha = a;
		beta = b;
		xk_1 = x_measured;
		vk_1 = 0;
	}
	void setAlphaBeta(double a, double b)
	{
		alpha = a;
		beta = b;
	}
	void getAlphaBeta(double * a, double * b)
	{
		*a = alpha;
		*b = beta;
	}
	double filter(double x_measured, double dt)
	{
		//update our (estimated) state 'x' from the system (ie pos = pos + vel (last).dt)
    xk = xk_1 + dt * vk_1; 
    //update (estimated) velocity  
    vk = vk_1; 
    //what is our residual error (mesured - estimated) 
    rk = x_measured - xk;  
    //update our estimates given the residual error. 
    xk = xk + alpha * rk; 
    vk = vk + beta/dt * rk; 
    //finished! 
     
    //now all our "currents" become our "olds" for next time 
    vk_1 = vk; 
    xk_1 = xk; 
		return xk_1;
	}
	
};

#endif
