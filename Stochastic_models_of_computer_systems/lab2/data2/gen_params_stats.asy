import graph;

void draw_stat_graph( string x_label, int var_idx, 
                      real x_min, real x_max )
{
  real dt = 1;

  string data_file_name = "gen_params.csv";
  //string x_label = "$SIGMA$";
  //int var_idx = 4;
  //real x_min = 0.3, x_max = 7.0;
  string y_label = "Relative Error";

  size(10cm, 10cm, IgnoreAspect);
  //scale(Linear, Log);

  file in = input(data_file_name).line().csv();

  real[][] a = in.dimension(0, 0);
  a = transpose(a);

  real[] in_N_V         = a[0];
  real[] in_dt_V        = a[1];
  real[] in_R_V         = a[2];
  real[] in_m_V         = a[3];
  real[] in_sigma_V     = a[4];
  real[] in_lambda_V    = a[5];
  real[] in_m_c_V       = a[6];
  real[] in_sigma_c_V   = a[7];
  real[] in_lambda_c_V  = a[8];
  real[] est_R_V        = a[9];
  real[] est_m_V        = a[10];
  real[] est_sigma_V    = a[11];
  real[] est_lambda_V   = a[12];
  real[] est_m_c_V      = a[13];
  real[] est_lambda_c_V = a[14];

  real [] err_R_V;
  real [] err_m_V;
  real [] err_sigma_V;
  real [] err_lambda_V;
  real [] err_m_c_V;
  real [] err_lambda_c_V;

  for (int i = 0; i < in_N_V.length; ++i)
  {
    err_R_V.push(abs(in_R_V[i] - est_R_V[i]) / abs(in_R_V[i]));
    err_m_V.push(abs(in_m_V[i] - est_m_V[i]) / abs(in_m_V[i]));
    err_sigma_V.push(abs(in_sigma_V[i] - est_sigma_V[i]) / abs(in_sigma_V[i]));
    err_lambda_V.push(
        abs(in_lambda_V[i] - est_lambda_V[i]) / abs(in_lambda_V[i]));
    err_m_c_V.push(abs(in_m_c_V[i] - est_m_c_V[i]) / abs(in_m_c_V[i]));
    err_lambda_c_V.push(
        abs(in_lambda_c_V[i] - est_lambda_c_V[i]) / abs(in_lambda_c_V[i]));
  }

  //draw("$R$", graph(a[var_idx], err_R_V));
  //draw("$m$", graph(a[var_idx], err_m_V));
  //draw("$\sigma$", graph(a[var_idx], err_sigma_V));
  //draw("$\lambda$", graph(a[var_idx], err_lambda_V));
  draw("$m_c$", graph(a[var_idx], err_m_c_V));
  draw("$\lambda_c$", graph(a[var_idx], err_lambda_c_V));

  xlimits(x_min, x_max);

  xaxis(x_label, BottomTop, LeftTicks(N=5), EndArrow);
  yaxis(y_label, LeftRight, RightTicks(N=7), EndArrow);

  //attach(legend(), point(NW), 40S+30E, UnFill);
}

// vim: ts=2 sw=2 et:
