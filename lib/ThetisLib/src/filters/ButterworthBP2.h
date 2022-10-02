#ifndef BUTTERWORTH_BP2_H
#define BUTTERWORTH_BP2_H

//Band pass butterworth filter order=2 alpha1=0.001 alpha2=0.25 
class ButterworthBP2 {
	public:
		ButterworthBP2() {
			for(int i=0; i <= 4; i++)
				v[i]=0.0;
		}
	private:
		float v[5];
	public:
		float step(float x) //class II 
		{
			v[0] = v[1];
			v[1] = v[2];
			v[2] = v[3];
			v[3] = v[4];
			v[4] = (2.951306830269793680e-1 * x)
				 + (-0.17158245437099103881 * v[0])
				 + (0.34833059957984097155 * v[1])
				 + (-1.17158245437099206576 * v[2])
				 + (1.99478805709291417969 * v[3]);
			return 
				 (v[0] + v[4])
				- 2 * v[2];
		}
};

#endif