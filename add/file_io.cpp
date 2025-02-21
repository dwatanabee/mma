#include <mma/MMASolver.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

void ObjSens(double *af, double *bf, double *x, int n, int m, double *f, double *df, double *g, double *dg);

using namespace std;

int main(int argc, char *argv[])
{
  std::cout << "start MMA" << std::endl;
  ifstream infile("input_mma.txt");

  // 最適化ステップ
  int iter;
  infile >> iter;

  // 要素数と材料数と制約関数の数
  int n, m, c;
  infile >> n >> m >> c;

  // 設計変数
  vector<double> s(n * m);
  for (int i = 0; i < (int)s.size(); ++i)
    infile >> s[i];

  // 目的関数の感度
  vector<double> dfds(n * m);
  for (int i = 0; i < (int)dfds.size(); ++i)
    infile >> dfds[i];

  // 制約関数
  vector<double> g(c);
  for (int i = 0; i < (int)g.size(); ++i)
    infile >> g[i];

  // 制約関数の感度
  vector<double> dgds(c*n*m);
  for (int i = 0; i < (int)dgds.size(); ++i)
    infile >> dgds[i];

  // 最小値と最大値
  double minv, maxv;
  infile >> minv >> maxv;
  vector<double>smin(n*m,minv);
  vector<double>smax(n*m,maxv);

  // 前設計変数
  vector<double> sold1(n*m), sold2(n*m);
  if(iter >= 2)
  {
    int counter = 0;
    double value;
    ifstream infileold1("old_s1.txt");
    while (infileold1 >> value) {
      sold1[counter] = value;
      counter++;
    }
    assert(m*n == counter);
  }
  if(iter >= 3)
  {
    int counter = 0;
    double value;
    ifstream infileold2("old_s2.txt");
    while (infileold2 >> value) {
      sold2[counter] = value;
      counter++;
    }
    assert(m*n == counter);
  }

  //sold1をold_s2.txtに入力
  if(iter >= 2)
  {
    ofstream outfileold2("old_s2.txt", std::ios::out);
    if (!outfileold2) {
        std::cerr << "error. cannot open file." << std::endl;
        return 1;
    }
    for (double value : sold1) {
        outfileold2 << value << '\n';
    }
    outfileold2.close();
  }

  //sをold_s1.txtに入力
  ofstream outfileold1("old_s1.txt", std::ios::out);
  if (!outfileold1) {
      std::cerr << "error. cannot open file." << std::endl;
      return 1;
  }
  for (double value : s) {
      outfileold1 << value << '\n';
  }
  outfileold1.close();
  
  // Initialize MMA
  MMASolver *mma = new MMASolver(n*m, c);

  mma->SetIter(iter);
  if(iter >= 3)
  {
    ifstream infileold1("old_s1.txt");
    for (int i = 0; i < (int)sold1.size(); ++i)
    infileold1 >> sold1[i];

    ifstream infileold2("old_s2.txt");
    for (int i = 0; i < (int)sold2.size(); ++i)
    infileold2 >> sold2[i];

    mma->SetXold(sold1, sold2);
  }

  // Call the update method
  mma->Update(s.data(), dfds.data(), g.data(), dgds.data(), smin.data(), smax.data());


  ofstream outfile("output_mma.txt", std::ios::out);

  outfile << n << endl;
  outfile << m << endl;

  // 設計変数
  for (int i = 0; i < (int)s.size(); ++i)
    outfile << s[i] << endl;

  std::cout << "end MMA" << std::endl;

  // Deallocate
  delete mma;
  return 0;
}
