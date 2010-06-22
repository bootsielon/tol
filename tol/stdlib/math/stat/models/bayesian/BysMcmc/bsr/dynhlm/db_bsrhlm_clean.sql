/* /////////////////////////////////////////////////////////////////////////////
  DB BsrHlm:

  Dynamic Hierarchical Linear Models to be estimated throught Bayesian
  Sparse Regression

  Cleans all tables related to BsrHlm

///////////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////////
Deleting tables order
///////////////////////////////////////////////////////////////////////////// */
DROP TABLE IF EXISTS `bsrhlm_v_est_param_stats`;
DROP TABLE IF EXISTS `bsrhlm_v_est_status`;
DROP TABLE IF EXISTS `bsrhlm_v_pri_output`;
DROP TABLE IF EXISTS `bsrhlm_v_pri_equ`;
DROP TABLE IF EXISTS `bsrhlm_v_lat_input`;
DROP TABLE IF EXISTS `bsrhlm_v_lat_output`;
DROP TABLE IF EXISTS `bsrhlm_v_lat_equ`;
DROP TABLE IF EXISTS `bsrhlm_v_lat_sigma_block`;
DROP TABLE IF EXISTS `bsrhlm_v_obs_transferFunction`;
DROP TABLE IF EXISTS `bsrhlm_v_obs_input`;
DROP TABLE IF EXISTS `bsrhlm_v_obs_arima_block`;
DROP TABLE IF EXISTS `bsrhlm_v_obs_output`;
DROP TABLE IF EXISTS `bsrhlm_v_mix_cnstrnt_lin_cmb`;
DROP TABLE IF EXISTS `bsrhlm_v_mix_cnstrnt_border`;
DROP TABLE IF EXISTS `bsrhlm_v_mix_order_relation`;
DROP TABLE IF EXISTS `bsrhlm_v_mix_non_lin_filter`;
DROP TABLE IF EXISTS `bsrhlm_v_mix_parameter`;
DROP TABLE IF EXISTS `bsrhlm_d_node`;
DROP TABLE IF EXISTS `bsrhlm_d_level`;
DROP TABLE IF EXISTS `bsrhlm_d_model_session`;
DROP TABLE IF EXISTS `bsrhlm_d_session`;
DROP TABLE IF EXISTS `bsrhlm_d_model`;
DROP TABLE IF EXISTS `bsrhlm_d_level_node_type`;
DROP TABLE IF EXISTS `bsrhlm_d_node_type`;
DROP TABLE IF EXISTS `bsrhlm_d_gibbs_block`;
/* */
