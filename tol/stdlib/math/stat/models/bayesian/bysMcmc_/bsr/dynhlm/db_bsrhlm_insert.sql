/* /////////////////////////////////////////////////////////////////////////////
  DB BsrHlm:

  Dynamic Hierarchical Linear Models to be estimated throught Bayesian
  Sparse Regression

  Inserts default registers to main dimensional tables

///////////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////////
  `bsrhlm_d_gibbs_block`
///////////////////////////////////////////////////////////////////////////// */
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('LinearBlk','Main linear regression');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('SigmaBlk','Variance of each segment of regression');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('ArmaBlk','AutoRegressive Moving Average');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('MissingBlk','Missing values of input and output');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('OmegaBlk','Numerator coefficients of a transfer function');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('DeltaBlk','Denominator coefficients of a transfer function and related initial and unknown values');
INSERT INTO `bsrhlm_d_gibbs_block` (`id_gibbs_blk`,`ds_gibbs_blk`)
VALUES ('NonLinearFilterBlk','Non linear filters');

/* /////////////////////////////////////////////////////////////////////////////
`bsrhlm_d_node_type`
///////////////////////////////////////////////////////////////////////////// */
INSERT INTO `bsrhlm_d_node_type` (`id_node_type`,`ds_node_type`)
VALUES ('OBS','Observational nodes are in the first level of hierarchical tree');
INSERT INTO `bsrhlm_d_node_type` (`id_node_type`,`ds_node_type`)
VALUES ('LAT','Latent nodes are in the second and upper levels of hierarchical tree');
INSERT INTO `bsrhlm_d_node_type` (`id_node_type`,`ds_node_type`)
VALUES ('PRI','Prior nodes stablish normality hypotesys about parameters of observational and latent nodes');
INSERT INTO `bsrhlm_d_node_type` (`id_node_type`,`ds_node_type`)
VALUES ('MIX','The mixture node is used to represent entities that are not linked to a specific node but to an unespecified set of nodes. If its used, it must be just one node called MIXTURE of type MIX at table `node`');

/* /////////////////////////////////////////////////////////////////////////////
`bsrhlm_d_level_node_type`
///////////////////////////////////////////////////////////////////////////// */
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('OBS',0);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',1);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',2);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',3);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',4);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',5);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',6);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',7);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',8);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('LAT',9);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('PRI', 888888888);
INSERT INTO `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`) VALUES ('MIX', 999999999);

