#pragma once
/***********************************************************************************/
/***********************************************************************************
double ARA_DEGER(double x, double y, double** TABLE, double alt, double del) {
	// [en boy]=size(TABLE);
	int en;
	int boy;
	double delX = del;
	double delY = del;
	double tempx = 1 + (x - alt) / delX;
	double tempy = 1 + (y - alt) / delY;
	int i = (int)floor(tempx);
	int j = (int)floor(tempy);
	double tx = tempx - i;
	double ty = tempy - j;
	if (i < 1 || i >= en || j < 1 || j >= boy) { return 0; }
	else {
		double Px1 = (1 - tx) * TABLE[i][j] + tx * TABLE[i + 1][j];
		double Px2 = (1 - tx) * TABLE[i][j + 1] + tx * TABLE[i + 1][j + 1];
		double Pxy = (1 - ty) * Px1 + ty * Px2;
		return Pxy;
	}
}
/*********************************************************************************/