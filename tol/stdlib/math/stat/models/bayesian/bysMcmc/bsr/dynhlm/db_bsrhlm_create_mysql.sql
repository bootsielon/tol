/* /////////////////////////////////////////////////////////////////////////////
  DB BsrHlm:

  Dynamic Hierarchical Linear Models to be estimated throught Bayesian
  Sparse Regression

  MySQL scritp that creates in current Data Base schema all tables needed to
  store Dynamic Hierarchical Linear Models to be estimated throught Bayesian
  Sparse Regression

  All table names are prefixed with bsrhlm_ to avoid conflicts with existent
  tables

  Data source is not handled by this system that is designed to store just 
  model definitions independently of output and input data at observational 
  level, that could be or not stored in the same or another data base in the 
  same or another data base server or even in other type of data storement 
  system (BDT, OIS, ...).

///////////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////////
Creation of tables is given in order of foreign keys constraints
User must insert data in the same order to ensure matching constraints
///////////////////////////////////////////////////////////////////////////// */


/* /////////////////////////////////////////////////////////////////////////////
The user should not touch this system table taht have all needed registers
///////////////////////////////////////////////////////////////////////////// */

CREATE TABLE  `bsrhlm_d_gibbs_block` (
  `id_gibbs_blk` varchar(64) NOT NULL,
  `ds_gibbs_blk` varchar(256) NOT NULL,
  PRIMARY KEY (`id_gibbs_blk`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores valid types of Gibbs blocks supported by BSR';



/* /////////////////////////////////////////////////////////////////////////////
The user should not touch this system table taht have all needed registers
///////////////////////////////////////////////////////////////////////////// */

CREATE TABLE  `bsrhlm_d_node_type` (
  `id_node_type` varchar(16) NOT NULL,
  `ds_node_type` varchar(256) NOT NULL,
  PRIMARY KEY (`id_node_type`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores valid types of hierarchical nodes';


/* /////////////////////////////////////////////////////////////////////////////
Stores all feasible hierarchical levels by node type. Observational nu_level
is set to 0, first latent level is 1 and maximum latent level is 9 by default,
but is possible to insert higher levels until 888888887, due to 888888888 is
reserved for prior level and 999999999 for mixed level.
The user could add registers for higher latent levels in very complex
hierarchical trees
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE `bsrhlm_d_level_node_type` (
  `id_node_type` varchar(16) NOT NULL,
  `nu_level` int(11) NOT NULL,
  PRIMARY KEY (`id_node_type`,`nu_level`),
  CONSTRAINT `UK_bsrhlm_d_node_type_level`
    UNIQUE KEY (`nu_level`),
  CONSTRAINT `FK_bsrhlm_d_node_type`
    FOREIGN KEY (`id_node_type`)
    REFERENCES `bsrhlm_d_node_type` (`id_node_type`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores all feasible hierarchical levels by node type.';

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


/* /////////////////////////////////////////////////////////////////////////////
Stores model name and a description of its objective. A model could have a lot
of implementations that will be referenced as sessions
The user must insert a register for each modelation target, not for each
implementation or session
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_d_model` (
  `id_model` varchar(64) NOT NULL,
  `ds_model` varchar(256) NOT NULL,
  PRIMARY KEY (`id_model`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores model name and a description of its objective.';

/* /////////////////////////////////////////////////////////////////////////////
Stores information about estimation sessions that are labels to distinguish
between different versions or implementation of one or more models
The user must insert a register for each session tag, that could apply over just
one specific model, a set of them or all together
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_d_session` (
  `id_session` varchar(64) NOT NULL,
  `ds_session` varchar(256) NOT NULL,
  `te_authors` varchar(256) NOT NULL,
  `dh_creation` datetime NOT NULL,
  PRIMARY KEY (`id_session`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores information about estimation sessions';

/* /////////////////////////////////////////////////////////////////////////////
The user must insert a register for each distinct application of a session tag
over a model
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_d_model_session` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`),
  CONSTRAINT `FK_bsrhlm_d_model_session_id_model`
    FOREIGN KEY (`id_model`)
    REFERENCES `bsrhlm_d_model` (`id_model`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_d_model_session_id_session`
    FOREIGN KEY (`id_session`)
    REFERENCES `bsrhlm_d_session` (`id_session`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores all allowed pairs of model-sessions';

/* /////////////////////////////////////////////////////////////////////////////
The user must insert a register for each distinct application of a session tag
over a model
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE `bsrhlm_d_level` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_type` varchar(16) NOT NULL,
  `nu_level` int(11) NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_type`,`nu_level`),
  CONSTRAINT `FK_bsrhlm_d_level_model_session`
    FOREIGN KEY                         (`id_model`, `id_session`)
    REFERENCES `bsrhlm_d_model_session` (`id_model`, `id_session`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_d_level_id_node_type_level`
    FOREIGN KEY                           (`id_node_type`,`nu_level`)
    REFERENCES `bsrhlm_d_level_node_type` (`id_node_type`,`nu_level`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores hierarchical selected levels by node type';

/* /////////////////////////////////////////////////////////////////////////////
The user must insert a register for each node of the model-session
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_d_node` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_node_type` varchar(16) NOT NULL,
  `nu_level`  integer NOT NULL,
  `nu_node` integer  NOT NULL,
  `ds_node` varchar(256) NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`),
  CONSTRAINT `FK_bsrhlm_d_node_model_session_node_type_level`
    FOREIGN KEY                 (`id_model`,`id_session`,`id_node_type`,`nu_level`)
    REFERENCES `bsrhlm_d_level` (`id_model`,`id_session`,`id_node_type`,`nu_level`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores hierarchical nodes';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to mixed nodes.
The user must insert a register for each variable of all gibbs blocks of all
nodes of each model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_mix_parameter` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_parameter` varchar(64) NOT NULL,
  `id_gibbs_blk` varchar(64) NOT NULL,
  `vl_prm_ini` double  NOT NULL,
  `vl_prm_min` double  NULL,
  `vl_prm_max` double  NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`id_parameter`),
  CONSTRAINT `FK_bsrhlm_v_mix_parameter_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_mix_parameter_gibbs_blk`
    FOREIGN KEY                       (`id_gibbs_blk`)
    REFERENCES `bsrhlm_d_gibbs_block` (`id_gibbs_blk`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores all parameters of regression';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to mixed nodes.
The user must insert a register for each non linear filter.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_mix_non_lin_filter` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_series` varchar(64) NOT NULL,
  `nu_lin_blk_param` integer NOT NULL,
  `nu_non_lin_blk_param` integer NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`id_series`),
  CONSTRAINT `FK_bsrhlm_v_mix_non_lin_filter_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores all non linear filters';


/* /////////////////////////////////////////////////////////////////////////////
This table is related to mixed nodes.
The user must insert a register for each relation as

  node_1.parameter_1 <= node_2.parameter_2

where pairs (node_1,parameter_1) and (node_2,parameter_2) must be stored as
parameters of main linear block of this model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_mix_order_relation` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_lower` varchar(64) NOT NULL,
  `id_parameter_lower` varchar(64) NOT NULL,
  `id_node_upper` varchar(64) NOT NULL,
  `id_parameter_upper` varchar(64) NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_lower`,`id_parameter_lower`,`id_node_upper`,`id_parameter_upper`),
  CONSTRAINT `FK_bsrhlm_v_order_relation_model_session_node_lower`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node_lower`, `id_parameter_lower`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`,       `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_order_relation_model_session_node_upper`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node_upper`, `id_parameter_upper`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`,       `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores order relations between linear block parameters';


/* /////////////////////////////////////////////////////////////////////////////
This table is related to mixed nodes.
The user must insert a register for each constraining inequation.
If field id_node is the MIXTURE then inequation affects to two or more distinct
nodes. In other case all parameters affects come from just that node called
id_node.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_mix_cnstrnt_border` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `nu_inequation` integer NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `vl_left_border` double NULL,
  `vl_right_border` double NULL,
  PRIMARY KEY (`id_model`,`id_session`,`nu_inequation`),
  CONSTRAINT `FK_bsrhlm_v_mix_cnstrnt_border_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores borders of constraining inequations.';


/* /////////////////////////////////////////////////////////////////////////////
This table is related to mixed nodes.
In this table id_node is never the MIXTURE but just one OBS or LAT node where
the parameter comes from.
The user must insert a register for each parameter affected by each
constraining inequation. Remeber that if
   constraint_border.id_node<>'MIXTURE' then
   constraint_border.id_node=constraint_lin_cmb.id_node for all
   constraint_border.nu_inequation=constraint_lin_cmb.nu_inequation
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_mix_cnstrnt_lin_cmb` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `nu_inequation` integer NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_parameter` varchar(64) NOT NULL,
  `vl_coef` double NULL,
  PRIMARY KEY (`id_model`,`id_session`,`nu_inequation`,`id_node`,`id_parameter`),
  CONSTRAINT `FK_bsrhlm_v_mix_cnstrnt_lin_cmb_model_session_nu_equation`
    FOREIGN KEY                                 (`id_model`, `id_session`, `nu_inequation`)
    REFERENCES `bsrhlm_v_mix_cnstrnt_border` (`id_model`, `id_session`, `nu_inequation`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_mix_cnstrnt_lin_cmb_model_session_node_parameter`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node`, `id_parameter`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`, `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores linear combinations terms of constraints.';


/* /////////////////////////////////////////////////////////////////////////////
This table is related to observational nodes.
The user must insert a register for each output serie at observation level of
each model-session.
Pair (id_node, id_series) will be used by a NameBlock series.handler to return
the corresponding TOL time serie variable, but data source is not handled by
this data base.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_obs_output` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_series` varchar(64) NOT NULL,
  `vl_mis_pri_sig_fac` double  NULL,
  `vl_mis_pri_min` double  NULL,
  `vl_mis_pri_max` double  NULL,
  `dh_start` datetime NULL,
  `dh_end`  datetime NULL,
  `vl_sigma` double  NULL,
  `vl_sigma_prior_weight` double  NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`),
  CONSTRAINT `UK_bsrhlm_v_obs_output_model_session_node_series`
    UNIQUE KEY  (`id_model`, `id_session`, `id_node`, `id_series`),
  CONSTRAINT `FK_bsrhlm_v_obs_output_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores observational output related information';


/* /////////////////////////////////////////////////////////////////////////////
This table is related to observational nodes.
The user must insert a register for each ARIMA factor attached to an output
serie at observation level of each model-session
AR and MA polinomial are given as text with maximum degree 2 in the standard way
'1'
'1 - fi_1*B'
'1 - fi_1*B - fi_2*B2'
If a high level n>2 is demanded then user can decompose it as n=2*f+1 is n is
even or n=2*f if is odd, and create f or f+1 factors respectively. If user wants
to force real roots its possible to create n factors of degree 1.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_obs_arima_block` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `nu_factor` integer NOT NULL,
  `nu_period` integer NOT NULL,
  `te_ar` varchar(1020) NOT NULL,
  `te_ma` varchar(1020) NOT NULL,
  `te_dif` varchar(1020) NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`nu_factor`),
  CONSTRAINT `FK_bsrhlm_v_arima_block_model_session_node`
    FOREIGN KEY                      (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_v_obs_output` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores ARIMA structures for observational outputs';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to observational nodes.
The user must insert a register for each input serie of each observation node of
each model-session.
Pair (id_node, id_series) will be used by a NameBlock series.handler to return
the corresponding TOL time serie variable, but data source is not handled by
this data base.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_obs_input` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_parameter` varchar(64) NOT NULL,
  `id_series` varchar(64) NOT NULL,
  `vl_mis_pri_sig_fac` double  NULL,
  `vl_mis_pri_min` double  NULL,
  `vl_mis_pri_max` double  NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`id_parameter`),
  CONSTRAINT `UK_bsrhlm_v_obs_input_model_session_node_series`
    UNIQUE KEY  (`id_model`, `id_session`, `id_node`, `id_parameter`, `id_series`),
  CONSTRAINT `FK_bsrhlm_v_obs_input_model_session_node_parameter`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node`, `id_parameter`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`, `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores observational input related information';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to observational nodes.
The user must insert a register for each transfer function
  omega(B)*y = delta(B)*x
of each observation node of each model-session.
Pair (id_node, id_series) will be used by a NameBlock series.handler to return
the corresponding TOL time serie variable, but data source is not handled by
this data base.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_obs_transferFunction` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_transferFunction` varchar(64) NOT NULL,
  `te_omega` varchar(1020) NOT NULL,
  `te_delta` varchar(1020) NOT NULL,
  `id_series` varchar(64) NOT NULL,
  `vl_mis_pri_sig_fac` double  NULL,
  `vl_mis_pri_min` double  NULL,
  `vl_mis_pri_max` double  NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`id_transferFunction`),
  CONSTRAINT `FK_bsrhlm_v_obs_transferFunction_model_session_node`
    FOREIGN KEY                      (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_v_obs_output` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores observational tramsfer functions related information';

/* /////////////////////////////////////////////////////////////////////////////
Stores sigma block information about latent nodes  .
If sigma is null then it will be simulated by Gubbs method
This table is related to latent nodes.
The user must insert a register for each latent node of each model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_lat_sigma_block` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `vl_sigma` double NULL,
  `vl_sigma_prior_weight` double NULL, 
  PRIMARY KEY (`id_model`,`id_session`,`id_node`),
  CONSTRAINT `FK_bsrhlm_v_lat_sigma_block_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores sigma block information about latent nodes';

/* /////////////////////////////////////////////////////////////////////////////
Stores output information about equations numbering latent nodes for each
model-session. Sigma_factor allows to define non constant covariance diagonal
This table is related to latent nodes.
The user must insert a register for each regression equation of each latent node
of each model-session. Each equation is attached to a linear combination of
child parameters at left side of equal sign, wich will be defined in
bsrhlm_v_lat_output; and other linear combination of father parameters at right
side of equal sign, wich will be defined in bsrhlm_v_lat_input.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE `bsrhlm_v_lat_equ` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_father` varchar(64) NOT NULL,
  `nu_equation` integer NOT NULL,
  `vl_sigma_factor` double NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_father`,`nu_equation`),
  CONSTRAINT `FK_bsrhlm_v_lat_equ_model_session_node_father`
    FOREIGN KEY (`id_model`, `id_session`, `id_node_father`)
    REFERENCES `bsrhlm_v_lat_sigma_block` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores information about equations numbering latent nodes';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to latent nodes.
The user must insert a register for each term (coef*parameter_child) of each
regression equation of each latent node of each model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_lat_output` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_father` varchar(64) NOT NULL,
  `nu_equation` integer NOT NULL,
  `id_node_child` varchar(64) NOT NULL,
  `id_parameter_child` varchar(64) NOT NULL,
  `vl_coef` double  NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_father`,`nu_equation`,`id_node_child`,`id_parameter_child`),
  CONSTRAINT `FK_bsrhlm_v_lat_output_model_session_node_father_nu_equation`
    FOREIGN KEY                           (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    REFERENCES `bsrhlm_v_lat_equ`         (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_lat_output_model_session_node_parameter_child`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node_child`, `id_parameter_child`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`,       `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores output information about latent nodes';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to latent nodes.
The user must insert a register for each term (coef*parameter_father) of each 
regression equation of each latent node of each model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_lat_input` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_father` varchar(64) NOT NULL,
  `nu_equation` integer NOT NULL,
  `id_parameter_father` varchar(64) NOT NULL,
  `vl_coef` double  NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_father`,`nu_equation`,`id_parameter_father`),
  CONSTRAINT `FK_bsrhlm_v_lat_input_model_session_node_father_nu_equation`
    FOREIGN KEY                           (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    REFERENCES `bsrhlm_v_lat_equ`         (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_lat_input_model_session_node_parameter_father`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node_father`, `id_parameter_father`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`,        `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores input information about latent nodes';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to prior nodes.
The user must insert a register for each scalar prior normal distribution of
each prior node of each model-session. Each distribution is attached to a linear 
combination of child parameters at left side of equal sign, wich will be defined
in bsrhlm_v_pri_output;
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_pri_equ` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_father` varchar(64) NOT NULL,
  `nu_equation` integer NOT NULL,
  `vl_average` double  NOT NULL,
  `vl_sigma` double  NOT NULL,
  `vl_sigma_prior_weight` double NULL, 
  PRIMARY KEY (`id_model`,`id_session`,`id_node_father`,`nu_equation`),
  CONSTRAINT `FK_bsrhlm_v_pri_equ_model_session_node_father`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node_father`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores output information about prior nodes';

/* /////////////////////////////////////////////////////////////////////////////
This table is related to prior nodes.
The user must insert a register for each term (coef*parameter_child) of each 
regression equation of each prior node of each model-session.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_pri_output` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node_father` varchar(64) NOT NULL,
  `nu_equation` integer NOT NULL,
  `id_node_child` varchar(64) NOT NULL,
  `id_parameter_child` varchar(64) NOT NULL,
  `vl_coef` double  NOT NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node_father`,`nu_equation`,`id_node_child`,`id_parameter_child`),
  CONSTRAINT `FK_bsrhlm_v_pri_output_model_session_node_father_nu_equation`
    FOREIGN KEY                           (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    REFERENCES `bsrhlm_v_pri_equ`         (`id_model`, `id_session`, `id_node_father`, `nu_equation`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_pri_output_model_session_node_parameter_child`
    FOREIGN KEY                         (`id_model`, `id_session`, `id_node_child`, `id_parameter_child`)
    REFERENCES `bsrhlm_v_mix_parameter` (`id_model`, `id_session`, `id_node`,       `id_parameter`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores output information about prior nodes';


/* /////////////////////////////////////////////////////////////////////////////
The user should not touch this system table taht have all needed registers
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_est_status` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `in_blocked` tinyint NOT NULL,
  `te_path_data_out` varchar(1024) NULL,
  `nu_mcmc_var` integer NULL,
  `nu_mcmc_burnin` integer NOT NULL,
  `nu_mcmc_sampleLength` integer NOT NULL,
  `nu_mcmc_cacheLength` integer NOT NULL,
  `nu_mcmc_currentLength` integer NOT NULL,
  `vl_mcmc_time_avg` double NULL,
  `nu_error` integer NOT NULL,
  `nu_warning` integer NOT NULL,
  `dh_loading` datetime NULL,
  `dh_parsing` datetime NULL,
  `dh_initializing` datetime NULL,
  `dh_simulating` datetime NULL,
  `dh_reporting` datetime NULL,
  `dh_evaluating` datetime NULL,
  `dh_saving` datetime NULL,
  `dh_ending` datetime NULL,
  PRIMARY KEY (`id_model`,`id_session`),
  CONSTRAINT `FK_bsrhlm_v_est_status_model_session`
    FOREIGN KEY                         (`id_model`, `id_session`)
    REFERENCES `bsrhlm_d_model_session` (`id_model`, `id_session`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores information about estimation process';

/* /////////////////////////////////////////////////////////////////////////////
The user should not touch this system table taht have all needed registers
If all works fine, system BSR-DynHLM will store here statistics about
simulated parameters for diagnostic purposes.
User will should insert nothing here.
///////////////////////////////////////////////////////////////////////////// */
CREATE TABLE  `bsrhlm_v_est_param_stats` (
  `id_model` varchar(64) NOT NULL,
  `id_session` varchar(64) NOT NULL,
  `id_node` varchar(64) NOT NULL,
  `id_parameter` varchar(64) NOT NULL,
  `id_gibbs_blk` varchar(64) NOT NULL,
  `nu_mcmc_index` integer NOT NULL,
  `vl_mean` double NULL,
  `vl_sd` double NULL,
  `vl_naive_se` double NULL,
  `vl_tmser_se` double NULL,
  `vl_q_001` double NULL,
  `vl_q_025` double NULL,
  `vl_q_250` double NULL,
  `vl_q_500` double NULL,
  `vl_q_750` double NULL,
  `vl_q_975` double NULL,
  `vl_q_999` double NULL,
  `vl_raftery_length` double NULL,
  `vl_raftery_burnin` double NULL,
  `vl_raftery_size` double NULL,
  `vl_raftery_size_min` double NULL,
  `vl_raftery_dep_factor` double NULL,
  `vl_raftery_remain` double NULL,
  PRIMARY KEY (`id_model`,`id_session`,`id_node`,`id_parameter`),
  CONSTRAINT `FK_bsrhlm_v_est_param_stats_model_session_node_parameter`
    UNIQUE KEY (`id_model`, `id_session`, `id_node`, `id_parameter`),
  CONSTRAINT `FK_bsrhlm_v_est_param_stats_model_session_mcmc_index`
    UNIQUE KEY (`id_model`, `id_session`, `nu_mcmc_index`),
  CONSTRAINT `FK_bsrhlm_v_est_param_stats_model_session_node`
    FOREIGN KEY                (`id_model`, `id_session`, `id_node`)
    REFERENCES `bsrhlm_d_node` (`id_model`, `id_session`, `id_node`)
    ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT `FK_bsrhlm_v_est_param_stats_gibbs_blk`
    FOREIGN KEY                       (`id_gibbs_blk`)
    REFERENCES `bsrhlm_d_gibbs_block` (`id_gibbs_blk`)
    ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1
COMMENT='Stores statistics about simulated parameters for diagnostic purposes';