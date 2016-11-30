package require wtree
package require BWidget
package require msgcat

namespace eval ::TolPkgGUI {

  namespace import ::msgcat::*

  variable widgets

  variable cwd [ file normalize [ file dir [ info script ] ] ]

  variable urlmap

  array set urlmap {
    http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php OTAN
    OTAN http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php
    http://packages.localbayes.es/BayesPrivateTolPackages/repository.php BPTP
    BPTP http://packages.localbayes.es/BayesPrivateTolPackages/repository.php
  }
  
  variable win

  variable packSyncInfo
  variable versSyncInfo
  variable localVersionInfo
  variable localMostRecent

  variable nodesInfo
  variable treeParents

  if { ![ info exists localInfo ] } {
    array set localInfo {}
  }

  if { [ lsearch [ image names ] green_light ] == -1 } {
    image create photo green_light -data {
      R0lGODlhEAAQAOedABoaGh0eHSUlJScnJykpKUtLS0xMTFxcXFxdXF1dXWNjY0l4KUt4LEt5LU57
      L1Z4QG1ubVd5QW9wb0iJG0mLG2yBX22BYFiQM3KEZ1eSMXaGa3OJZHSJZ3mHb4KDgYOEgoSEhISG
      g3yKdIWGhHCRWoWHhHCTWIeIhlegJliiJnSUYF+jM3eZYY6OjmClM1+oL2CqLouXg3ufZo2YhoCh
      bIGibZqbmpubm4mldpucm4emdZifk52dnZ2fnJ6fnI6ofZ2jmJ+inZGqgXO4UaCknpSshHS7UaSm
      oqSmo4e4ZaqsqKSvnKutqqSxnI69bK2urHfPPLGzr3jQPIjLW7S2sn3URX3VQ4zOYLa6s4/OdYLW
      Wbm9tYTYUILYWJHRdoTZTp/NiqDNjL3Au7/AvsDAwL/BvsHBwcDDvsPDwrfKrMTEw7nKr4riUrzK
      sovjVL3Ls5DkXZHkX5TmaJbmZ5Xma5fmZ67anJfmaZnnbZ3ncM/Pzpzoc53odNHR0ZrqgqHpeqXo
      eaHpe6Xoep/qgJzrhaHqgqXpg6XphNTU1K7rh9fX1q/ridfY19TjycHto9fmzcPwpcPwpsTwp9fv
      xdfvxtnzx9X2zNr1yNb2zuL52+P53ev25Oz35P//////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAQABAAAAjfAP8JHHiDjBkeAxMO7HNmhoUKMcQgUiiQkYgHQtq8KRKh
      gyKFejQwaLSJE6dNjxxg6DMQBJAGSShVunSp0iQnC3a0EIiGw4QpjiJJkgTJ0RUKG9QIfKICBRRA
      iRYtSiRISgoSTAQiYfGiips6efLUYWMFhokjAnv8WMElzh08eObA+eIChw+BNpZc0EKHT6A/e+R0
      ydAkh8ADQXQMIVTokKFBfozUIKJg4AgsNLJg0pTJkhcZW04kFOAhypowduyASUPlAwGFACCEUDKm
      DJMSEgJQFCigAIIEBgbsHk4xIAA7
    }
  }
  
  if { [ lsearch [ image names ] ok_green_16 ] == -1 } {
    image create photo ok_green_16 -data {
      R0lGODlhEAAQAOelAEhfPU5hRlFiTFNiTk5oP1NmS1VlUFRmTVllWFloUlxoW15rXF1sV1dwSVpy
      S11yUmlyaGl0ZWxzbmB6UGaET1yOMXqCeWqLUV6XLH+Ie1KhCFOhCFOiCFSiCFWiCVejDVekDFik
      DnGWVFqkEVilDlmmD1ymFF2nFl6nGGCpGoqSimGqHHWkTnijVGyxKm6yLmm7G3O2M3W2N5qimXyz
      SWq/GZ6jnpykm3e5OH27Qn29P36/QYS9UoLBR3DNF3HOFnLQFonFT3PSFnTSGHjTHo/HW3jUHnjU
      H3rUIXvVJLO3s5TKYX/WKpTLXbS5tJHNV5LOWIXWNpPPWbe8t5DSUYjZOYnZOorZPLu+vY/XSIra
      PZXUWozaQJnTYY3aQY/ZRo3aQp/Rbo7bQ4/bRJDbRpDbR5vVZJDcRpLcSZLcSpTbT5PcS5PcTJvY
      XpTcTZvZX5TdTZTdTpbdUMHFxJfdVJfeUpjeU5rdWpneVZvdWpzdXpveWpzeW5rfV6DcZZ7dYaPb
      a5zfWp3fW6HdaKfadZ3gXJ/fYZ/gX6DhYaHhY6fecqLhZKPhZazcfaPhZq3cf6PiZqzfe63hfKvk
      c8/Rz63kd7Higa7kd9DS0bDlfrPkgrTkhrrmj9ja2dvb29zd3eDh4OTl4+Xm4+7v7vLy8f//////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAQABAAAAjmAP8JHPiPAweCCAV+6NHFzyAzQUYk/OdCkZsqVq5wwSNJ
      BsIXlc6QQbMGjpwLFBrlGBjCEh48fQIVOlQEApYBnFAIXDJpkSNGixIhevDJRgtIYQQ+MiTiwBZB
      gXhYGGWAjp1IBTexkEBJAJU6E0CpoMEmTSYPHTSpceAEU4EsC0glGCMGzCUQ/wiV+dJgihIAM+ZU
      MJKECSAN/5rE0RIlQKcbnjLUECLkCBSBJv4MEQJDQShRDCgLuZNiYAw2lDEgiECAshcdCHHoQSLk
      hw8gRPLsmHjiSRs+e95IWTFx4AYSJTYkDAgAOw==
    }
  }
  if { [ lsearch [ image names ] yellow_light ] == -1 } {
    image create photo yellow_light -data {
      R0lGODlhEAAQAPcAABoaGh4eHSUlJScnJykpKX9qOUtLS0xMTFxcXF1dXF1dXWNjY25ubXBwb5ds
      DZptDIJkH4JlIoNmI4VnJZ56JZx5J4BrOoV5W4V5XId9Y4l/aLOBFbCAFriJH7qKHrKHJbSIJOKp
      KeOqKeexMuixMeu5POq4PpuEUZmEUpuHWaCLWqaSX46AYI2BY4qBbI2EcaiWZamWZqybbquacK6e
      eK+ge7GifsWmQ8mpQ9mtTMOiWtywUcenYujDR+bCSfXAQPXAQfXFTPXGTvbJVvbJV/bKV/bLWPbO
      WPbPW/fNXffOXtrDat3HavfNYffMa/fNa/fQZPfRZffUbPjUbffccvbYdvbYd/jddfjSefjTevjU
      fPjmdfjneISDgYWEgoaFg4SEhIaFhIeGhIiIho6OjpmSgZqTg5uampubm5ycm52dnZ+enKGckqCe
      nKWgl6KgnKSinbGrmrSsmaeloaemo6yrqK2sqa2sqq6tq7Syrre1sbu5sr67tMG/u9bEgdbEg+LT
      lPfamfrdnPrdnc7EqM3Drs7FrM3Er8HAvcHAvsPBvsDAwMHBwcPDwsTEw8/PztHR0dLR0dTU1NjX
      1tjY1+fcxenfyvXkwPnowfvpwvz1xvz1yP342P342fjx4vny4gAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAKAALAAAAAAQABAA
      AAjfAEEJHIhmESM1AxMOhKTIDIYLZfpIUiiQ0osCNQodsmHBxSSFjzREqOTp0ydPliZkiDQQjBsJ
      Oi5hypQJ0yUeENiQEdiohYMcgQQNGiQo0I4HLBwJxJOCQwgnWbRowfJExAYUdwTSUdFhBBAiTZoM
      +UHCw4k5AtfQ+GBCiBElSYoEKQFiRhuBZ+JU8IEkyhQpUI70oCAnjUAEb2Tc4HLFShUqW3DEgLNg
      YJg9MJZs6sRJE5MVfMYkFNAlj6E/gAD5IaTHCwGFABh8qYMokR0xDQJQFCjAQAIFBwbsHk4xIAA7
    }
  }

  if { [ lsearch [ image names ] refresh_yellow_16 ] == -1 } {
    image create photo refresh_yellow_16 -data {
      R0lGODlhEAAQAMZJAI99Ob2SKdGaI9SbI8+fIOCjHbynYsSpSLypbeKmHL+qZdWsGryrd+upGdyw
      FsaycuS0E+yzE+W3E/u0Evu1EfK4D+i7GPu3EPy4D/y4EPy6Dvy7Dfy9DPy+DP2+C/3ACv3BCv3C
      Cf3DCP3DCf3ECP7ECOTEWfzGB/7GBv7GB/7HBv7JCP7JCf3JDP7KDPHJPf/MEP3MGf/MFPjMLf/N
      Fv/NGv/PIf7PJf7QJ//QKv/SMv/TNv/TOP/VQ/7WR/7WSPjXaP/YU//YVf/ZWP/bZP/bZv/edf/e
      d/7ggf//////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////yH+
      EUNyZWF0ZWQgd2l0aCBHSU1QACH5BAEKAH8ALAAAAAAQABAAAAdpgH+Cg4SFhoeIf0gKiYMPRkRB
      PTo2MBKHQEM+OzcyKykjFYcvOTEtJyIfHBoBhxYAgwQZEw2NjQwIBraCR0VCB4MOEIcmPzw4NCwo
      qRGHMzUuKiUgHRoXCYcLJCEeGxgUBbt/AgPi5o2BADs=
    }
  }

  if { [ lsearch [ image names ] red_stop_16 ] == -1 } {
    image create photo red_stop_16 -data {
      R0lGODlhEAAQAOeLAKkAAKoAAKoBAasEBKsGBqwGBqsHB60ICMwAAKwJCcwCAq0NDa8PD68QEM4I
      CMcMDLATE84MDLEUFLYbG4YpKoUrK4UtLbcjI4c0NLgnJ7srK4g7O9MnJ9QnJ7wvL5E7O7swMNQq
      Kr4wMNUuLtUvL7w4OME3N8I4ONc0NMA7O9c1Ndc4OMU+Ptg6Otg7O9g9PcdCQtk/P9lCQshHR9pE
      RNpFRdpGRtpHR9pISM5NTdpKSttKStpLS89OTtlMTNBPT9tNTdpPT9xPT81UVNxQUNZSUtdSUtxR
      UdxSUtNVVd1UVNpVVc1ZWd1VVYFubtVZWYRvb91XV91YWNpZWd5YWNpbW9pgYNRlZdxmZuBmZtpr
      a+Fra91xcd9xceJyctx2dpuIh5aKiI+NieB3d+R2duF3d56LiuJ7e6COjaOSkaaVlOSFhaiYl6eb
      maubm6KfnKydnLKpp6+tqru+uL/CvcPFwMfJxcvNyc7QzNDSzdLU0dPV0dbY1NfY1Nrc2Nzd2d7f
      3N/h3uLj4OPk4eXm5Obn5enq6Orr6e7u7fHy8fX19P//////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAQABAAAAjXAP8JHPgvQACCCAUSYJFkSpUnMxIk/HcBywsOHUKMqNHF
      A8IMW1CoWNHiRQwZNsikGGhgDI0aN3YAIYKkCZUoaxoIZJKlSZQoTZQgOUJECBApVwR+QQIFjhs2
      atKgMQPGCQwuBc/giKMoEaJDhQYF+iOnhBcBAcrEaGOIkCBAfvrsyfMGxJYB/7SoCMNHD547durQ
      mSNmghUA/4a8+LABg4UKkCFTeNBD4IIlChBo3rzZB4SBGlxw5kzCBEIRQSJwdsDjxEQGOYoA0WHk
      h4SJAwEUOIAYYUAAOw==
    }
  }

  if { [ lsearch [ image names ] yellow_stop_16 ] == -1 } {
    image create photo yellow_stop_16 -data {
      R0lGODlhEAAQAOeQAIWFbpSLK4iIb5SMLZaNKpSNNJWOO5+WO5CTiZiaiZ6fh6GiiqOkjcqnDaKl
      nMqoDcuoDcupDMupDaankcupE8yqEMuqE8yrEsyrFqiplKmpmc2sFM2sGs6uGs2uHausl8+vHq6u
      m9CxIK+vnK+yqtS3LrO1p9W4MtG+G9e8NNe8OdHBMNq/ONLDOLu+uNrBRNzDP93EQL/Cvd7GR8PF
      wODKTubPDN3NQsfJxe3UAO3UAuPPWuPPYO7VCMvNyebRUubRU+7WDOjTU87QzOnWWdDSzfDaJ+jW
      afDbJ/DbKurYXdLU0fDcLvDcL9PV0e3aVPHdNO3bVPHdNfHdOPHeOvHeO/HePfHfP9bY1O/dW9fY
      1PLfQu/eVu/dYfLfRfDfTPHfS/HfUO7dbfLgRPLgRfLgRvLgR/LgSO/fXPLgSvLgTfLhSvLhTfPh
      T/PhUPDgaO7fevPiUPPiUfPiUtrc2PPiVPPiVfDgc/PjV/PjWNzd2fLjcvHjeN7f3PTlZvTla/Tm
      a/Pled/h3vPlffTmcuLj4PbodvTohuPk4eXm5Obn5enq6Orr6e7u7fHy8fX19P//////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAQABAAAAjWAP8JHPjvAQSCCAVSmEEkCxolNTAk/FfijRUjSJIw8bJH
      BcITgKBImULFypUtZQy9GGiBzxgyZtawcTPHTh48hzwI5OHHDh48durMkROnDZs8RwTCmSNgRIgP
      GSYwWKAAwI07/yAMOmPikaNGjBQhEqSHRAtCEiIEuqJhUaJCfehocVLEwYo/Ff6JkZIAy5IhPnDQ
      kOECAYouAndYOWCgwIAAkCETsAFEIAcuOnJo3rz5C4iBKapw5twEBkIWYYJw7gEmxsQOP56oSRNF
      iIiJBC9saJAwIAA7
    }
  }

  if { [ lsearch [ image names ] red_light ] == -1 } {
    image create photo red_light -data {
      R0lGODlhEAAQAPcAABoaGh4dHSUlJScnJykpKUtLS0xMTFxcXF1cXF1dXWNjY25tbXBvb54KB4ge
      GqEJBrgTD7sSDocgHYgiH6QkH7siHbkkHoojIIM4NoQ5NqInIsIbFsAdGOwmIO0mIPEvKPAxKfM/
      NvQ9NMxLPdBLPf5AN/5BOPFSP55QTZxST4dbWYhcWp5YVqRZVpBgXqpgXIljYotoZoxsa5BjYY5x
      cKtnYq1mY7Bva65wbrF3dbJ7ebR+fMhXVcxfXeFJReNOS/5ORO5TQf5QRv9XTv9YT/9aT/9bUf5f
      UP5iU/9hVv9iVv9iWf5pXf5rXv5pZP5qZeB1ZON3Zf9yZf9yZv5/bP95c/1+cP1/cf95dP98dtuI
      fdqKf/6Cb/+Lbv+PcoSBgYWCgoaDg4SEhIaEhIiEhImGho6OjpqBgJyDgpuampubm5ybm52dnZ+c
      nKKRkaWXl6CcnKOcnKWdnbOambaamaehoaejo6yoqK6pqa6qqq+rq7Svrreysbyzsr+1tOadkOed
      kPyXlPyYlf6al/6bl/+cmc+trc+urtCpptCtqsG6u8G9vcG+vsS+vve/vvi/vvzAv8DAwMHBwcPC
      wsTDw8/OztHR0dLR0dTU1NjW1tjX1+jExOvJyP7BwP/RxP/Sxv/e1v/g2Pnh4fvh4QAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAKQALAAAAAAQABAA
      AAjfAEkJHKgmkiQ2AxMOtNQIzQoVZxRhUihQEw0MOgwd2pFBRiaFlWJI2CRq1ChRnC7AuDRQzJsJ
      PBxB6tQJ0qMeDtyYEThpRgMfgggVKjQo0I8HLigJ1MMCQgcnWLJkqfLEQ4QUeQTaacEBhAkiS5YM
      KfFhA4o6AtvksBBCiBElSYoAEVEBBxyBaeZoCIKkyRQpTI6coEBnjcADcW6M8MLlihUqXUjYkKNg
      4Jg+NaB8CgXKU5QXfsokFPBlT6ItfwBpQcQHDAGFABaEubOIER4yDAJQFCigAIIEBgbsHk4xIAA7
    }
  }
  
  if { [ lsearch [ image names ] database_refresh_32 ] == -1 } {
    image create photo database_refresh_32 -data {
      R0lGODlhIAAgAOf/ADRnADpsADpyAD53AEJ6BD18ADd/AEV7ADmAADKDADqBADuDAD2EAESCAD+G
      AmtvckeFA0GHBUOJAGtzekaLAHJyh0iNAE6MD0qPAFWRB1uKO1SWAF6RM36Dhl2ZFWeUPn6GjnyI
      iFihAIGFoIGIkGChAGWfEX2MkoeHqW6YUIWNlYOPkG2hIoePloiQmHWlAHylAJCQpnKqAHSiS46S
      oXGkRW+kTIyUm4aWnHmlSIGqBIKrAH6tAIuaoXisLnSyAH2pS5KaonyqU4KxAJScpJWdpZaeppCg
      pnq4AIOtY4OvUZigqIG4BX26AJmhqYKyRZmgtZOjqZmzAIurgpujq4G+AJ22AITAAJ28BKKqspS0
      eZrAAKCsuYTHAJXCAKOstJHEAKSttZjEAKautpq0n57DAJa+Up22j47FK5rGAKSwvpvIAJ3JAK2x
      wKqzu6qyx5/DQJnMBKK7k6HMAK+zz5zOAKHNDJ7QAKjEYaTPAJnTAKPPEqDSAK66x5XWAKHTAKy/
      q7O7w7HLOrS70Z7XAK3RGafTGLHQKLa/x7jAyLnByZ/eALrCyrTHs7rC17vDy7zEzL3FzajgAL7G
      z57lAL/H0L/G3LzI1sbG3cDJ0cHWVq/hLL/L2cLL08LK38PM1MnJ7cTN1azqAMTaWrLkMMbO1sjN
      3MjM6cLdVMPP3cfP18XcY8TQ3s3N5cjQ2MbfTsnR2svP7MneXsbS4NHQ6MvT3Mjgbs/U18zV3czU
      6tDU5M3W3tDhcMrjcM7X38rmZM/Y4NDX7c3Z59DZ4djic9LpR9fX79HpUNLa4svobdbjgdPb5Nba
      6tPb8dTc5drb5dXd5tXmkNTohNbe59ne7tnojNfoktnd+tfg6Nvf797e9dnh6tzg8OTonOLootvj
      7Nzsltno29zk+t3l7uTj++Hk/9/n8O3sqOPn9+Dp8eTp7OHq8ufvr+fn/+Ps9Orq/+jt8PHys+ru
      /vXzwuzx9Ory+/Dx++7z9vb2//P4+/T5/PX6/fb7/vj+//7//P///yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAgACAAAAj+AP8JFHhkyRInCBMqNHhkoMOH/4xEGZPIFa505jJqzJgO
      l6tEY5wYgTiwyBhc26ZBQ8aypUuW0KZtQzamCMl/UXbtwlWrp8+fQGvh0hnl5pFxnz6VWsq0qdNS
      Scc1JFmE37dMjyBBisS1q1etjzJ940fFaKR7++rNVNWprdu2qpBtq5fWFBSjgRJ1QrYOnj59+QIL
      /gtPHbJPjhJ8uGkkEaNHkTJ1KuXKF7KYyHzhYpWKU6ZLmBjoaEBVESOuk13tAoZsGrZnzJgFy0Wr
      1SlQDIjpYADxxBjIkimvRoaNWzZhSTIgWK6AAQJlo3iQdqgiyxhFqWtZRpaNnBYPZqL+gStW7Nix
      YtHEyBoCwWGQdWOIOHETKFInYN2BPLGGqtCePAACmMaAv1jgUA+d0LPOLYy4kQUXXLyRwhPSGBLH
      HXxkqOEdGDJxwIGJJFIKNPDQww8//YSTQTJ/6KHHFSVQwMCMDBBCSBMDPNSDaVtl8okquDQzAxqb
      +EFICRecAUgstzEgSRU5PhQEI6fZR5kv2iBAyiKLdHFBLrmY8gloDnQR5UMgjNFjasAIg4AolFAi
      ghzBzFJKJpFYwsCZaFqHHWW1AAOMcggooEAjxujCSieZ9PHGCP9YQAEFFhj4zw3uxDdffZxkI047
      77RTzjU1JDAIF06M4c5dLMzTzTnuluLwiYIMOgihGmq08QYdHMDAgCe+jGOPKTQogIc3VlgRa4gj
      lsjPPtDuw48/SrwQDwP4kJMKGVMw4AM7gmCxQwEC3cBjZD9qx105NrxQTTWFMmcCHNZoskUZll5K
      JWrCAfOMODnIwIsYthRsMC+rHFJGGhsIMBAOikBmpWr+koPAKwOmYcfGGw+YhxgbEOBQB0EoEpwr
      texyGcA/8MGhhhn+AYYMFgAA0QQtBJFFII+whs036MgjBBI2MhCBBJNSAEEANw30QAgktNDCCitI
      HYMGVUhCAQoVNO31TQN0ke/XZDt0wNhlp6322moHBAA7
    }
  }
  if { [ lsearch [ image names ] install_zip_32 ] == -1 } {
    image create photo install_zip_32 -data {
      R0lGODlhIAAgAPcAACUmJSYoJikpJyorKD4/PFA0Clg7DEY2Gl0/Ez9BPm5EBmpECXBHCXJJC2VF
      FWlIF2lJGHROEFdMNWtQJFBRTVpZUlxeWl9iXnpsUn5zXGJnYWlqZ21wbndwY3FybXx3and4dXt8
      eJ9pEJJkG5hnG5loHKBmCqJoDqJtFKl0HpZpJZZrK4xpMKp1IqFzL698LLF8KaF1MqN4NopvR5F0
      RJ58RIZ2WaB+SbaBLqmAP7WCNbWDOLuKPcaRP8+WPqWDS7WJRLiLRKeGU66LVK+PXbGPW7eRVrOR
      WoGDf52Oda6RY7KTYbyZYr2caKuTcMeSQ82WQ8qXSsyaTtObRtSeS8OXUc+cUcacXNCfU9miTs6h
      XNKiV9mjUdSlXdypW+CsXcOdYMmiZMikbNytZNKqa9uvad2wa9Svddy0ddi0fOGuYOa0Zum5buG1
      cOa5c+y9dISFg4eJhYuLi52UhZSSiZSVlJeYlZiZl5eZmJydm6idjqukl6Chn6qmnqSlo6appauo
      o6anqaepqqurqrCtpLOzramtsbS0s7i5t7u8u926hb7BvODAjcDBvvfVoPncr8XFxMfIxsnKx8vM
      y9LPx8/QzsjL0NTU09fZ09nZ19bY29zc297g3+Dh393e4N/i5uTl4+fo5+nq5+bn6ezs6/Dx7vT0
      8/n59/f4+P39/QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAKoAIf8LSUNDUkdC
      RzEwMTL/AAAMSExpbm8CEAAAbW50clJHQiBYWVogB84AAgAJAAYAMQAAYWNzcE1TRlQAAAAASUVD
      IHNSR0IAAAAAAAAAAAAAAAEAAPbWAAEAAAAA0y1IUCAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAARY3BydAAAAVAAAAAzZGVzYwAAAYQAAABsd3RwdAAAAfAA
      AAAUYmtwdAAAAgQAAAAUclhZWgAAAhgAAAAUZ1hZWgAAAiwAAAAUYlhZWgAAAkAAAAAUZG1uZAAA
      AlQAAABwZG1kZAAAAsQAAACIdnVlZAAAA0wAAACGdmll/3cAAAPUAAAAJGx1bWkAAAP4AAAAFG1l
      YXMAAAQMAAAAJHRlY2gAAAQwAAAADHJUUkMAAAQ8AAAIDGdUUkMAAAQ8AAAIDGJUUkMAAAQ8AAAI
      DHRleHQAAAAAQ29weXJpZ2h0IChjKSAxOTk4IEhld2xldHQtUGFja2FyZCBDb21wYW55AABkZXNj
      AAAAAAAAABJzUkdCIElFQzYxOTY2LTIuMQAAAAAAAAAAAAAAEnNSR0IgSUVDNjE5NjYtMi4xAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABYWVogAAAAAAAA
      81EAAf8AAAABFsxYWVogAAAAAAAAAAAAAAAAAAAAAFhZWiAAAAAAAABvogAAOPUAAAOQWFlaIAAA
      AAAAAGKZAAC3hQAAGNpYWVogAAAAAAAAJKAAAA+EAAC2z2Rlc2MAAAAAAAAAFklFQyBodHRwOi8v
      d3d3LmllYy5jaAAAAAAAAAAAAAAAFklFQyBodHRwOi8vd3d3LmllYy5jaAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABkZXNjAAAAAAAAAC5JRUMgNjE5NjYtMi4x
      IERlZmF1bHQgUkdCIGNvbG91ciBzcGFjZSAtIHNSR0L/AAAAAAAAAAAAAAAuSUVDIDYxOTY2LTIu
      MSBEZWZhdWx0IFJHQiBjb2xvdXIgc3BhY2UgLSBzUkdCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGRl
      c2MAAAAAAAAALFJlZmVyZW5jZSBWaWV3aW5nIENvbmRpdGlvbiBpbiBJRUM2MTk2Ni0yLjEAAAAA
      AAAAAAAAACxSZWZlcmVuY2UgVmlld2luZyBDb25kaXRpb24gaW4gSUVDNjE5NjYtMi4xAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAB2aWV3AAAAAAATpP4AFF8uABDPFAAD7cwABBMLAANcngAAAAFY
      WVog/wAAAAAATAlWAFAAAABXH+dtZWFzAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAACjwAAAAJz
      aWcgAAAAAENSVCBjdXJ2AAAAAAAABAAAAAAFAAoADwAUABkAHgAjACgALQAyADcAOwBAAEUASgBP
      AFQAWQBeAGMAaABtAHIAdwB8AIEAhgCLAJAAlQCaAJ8ApACpAK4AsgC3ALwAwQDGAMsA0ADVANsA
      4ADlAOsA8AD2APsBAQEHAQ0BEwEZAR8BJQErATIBOAE+AUUBTAFSAVkBYAFnAW4BdQF8AYMBiwGS
      AZoBoQGpAbEBuQHBAckB0QHZAeEB6QHyAfoCAwIMAv8UAh0CJgIvAjgCQQJLAlQCXQJnAnECegKE
      Ao4CmAKiAqwCtgLBAssC1QLgAusC9QMAAwsDFgMhAy0DOANDA08DWgNmA3IDfgOKA5YDogOuA7oD
      xwPTA+AD7AP5BAYEEwQgBC0EOwRIBFUEYwRxBH4EjASaBKgEtgTEBNME4QTwBP4FDQUcBSsFOgVJ
      BVgFZwV3BYYFlgWmBbUFxQXVBeUF9gYGBhYGJwY3BkgGWQZqBnsGjAadBq8GwAbRBuMG9QcHBxkH
      Kwc9B08HYQd0B4YHmQesB78H0gflB/gICwgfCDIIRghaCG4IggiWCKoIvgjSCOcI+wkQCSUJOglP
      CWT/CXkJjwmkCboJzwnlCfsKEQonCj0KVApqCoEKmAquCsUK3ArzCwsLIgs5C1ELaQuAC5gLsAvI
      C+EL+QwSDCoMQwxcDHUMjgynDMAM2QzzDQ0NJg1ADVoNdA2ODakNww3eDfgOEw4uDkkOZA5/DpsO
      tg7SDu4PCQ8lD0EPXg96D5YPsw/PD+wQCRAmEEMQYRB+EJsQuRDXEPURExExEU8RbRGMEaoRyRHo
      EgcSJhJFEmQShBKjEsMS4xMDEyMTQxNjE4MTpBPFE+UUBhQnFEkUahSLFK0UzhTwFRIVNBVWFXgV
      mxW9FeAWAxYmFkkWbBaPFrIW1hb6Fx0XQRdlF4kX/64X0hf3GBsYQBhlGIoYrxjVGPoZIBlFGWsZ
      kRm3Gd0aBBoqGlEadxqeGsUa7BsUGzsbYxuKG7Ib2hwCHCocUhx7HKMczBz1HR4dRx1wHZkdwx3s
      HhYeQB5qHpQevh7pHxMfPh9pH5Qfvx/qIBUgQSBsIJggxCDwIRwhSCF1IaEhziH7IiciVSKCIq8i
      3SMKIzgjZiOUI8Ij8CQfJE0kfCSrJNolCSU4JWgllyXHJfcmJyZXJocmtyboJxgnSSd6J6sn3CgN
      KD8ocSiiKNQpBik4KWspnSnQKgIqNSpoKpsqzysCKzYraSudK9EsBSw5LG4soizXLQwtQS12Last
      4f8uFi5MLoIuty7uLyQvWi+RL8cv/jA1MGwwpDDbMRIxSjGCMbox8jIqMmMymzLUMw0zRjN/M7gz
      8TQrNGU0njTYNRM1TTWHNcI1/TY3NnI2rjbpNyQ3YDecN9c4FDhQOIw4yDkFOUI5fzm8Ofk6Njp0
      OrI67zstO2s7qjvoPCc8ZTykPOM9Ij1hPaE94D4gPmA+oD7gPyE/YT+iP+JAI0BkQKZA50EpQWpB
      rEHuQjBCckK1QvdDOkN9Q8BEA0RHRIpEzkUSRVVFmkXeRiJGZ0arRvBHNUd7R8BIBUhLSJFI10kd
      SWNJqUnwSjdKfUrESwxLU0uaS+JMKkxyTLpNAk3/Sk2TTdxOJU5uTrdPAE9JT5NP3VAnUHFQu1EG
      UVBRm1HmUjFSfFLHUxNTX1OqU/ZUQlSPVNtVKFV1VcJWD1ZcVqlW91dEV5JX4FgvWH1Yy1kaWWlZ
      uFoHWlZaplr1W0VblVvlXDVchlzWXSddeF3JXhpebF69Xw9fYV+zYAVgV2CqYPxhT2GiYfViSWKc
      YvBjQ2OXY+tkQGSUZOllPWWSZedmPWaSZuhnPWeTZ+loP2iWaOxpQ2maafFqSGqfavdrT2una/9s
      V2yvbQhtYG25bhJua27Ebx5veG/RcCtwhnDgcTpxlXHwcktypnMBc11zuHQUdHB0zHUodYV14XY+
      /3abdvh3VnezeBF4bnjMeSp5iXnnekZ6pXsEe2N7wnwhfIF84X1BfaF+AX5ifsJ/I3+Ef+WAR4Co
      gQqBa4HNgjCCkoL0g1eDuoQdhICE44VHhauGDoZyhteHO4efiASIaYjOiTOJmYn+imSKyoswi5aL
      /IxjjMqNMY2Yjf+OZo7OjzaPnpAGkG6Q1pE/kaiSEZJ6kuOTTZO2lCCUipT0lV+VyZY0lp+XCpd1
      l+CYTJi4mSSZkJn8mmia1ZtCm6+cHJyJnPedZJ3SnkCerp8dn4uf+qBpoNihR6G2oiailqMGo3aj
      5qRWpMelOKWpphqmi6b9p26n4KhSqMSpN6mpqv8cqo+rAqt1q+msXKzQrUStuK4trqGvFq+LsACw
      dbDqsWCx1rJLssKzOLOutCW0nLUTtYq2AbZ5tvC3aLfguFm40blKucK6O7q1uy67p7whvJu9Fb2P
      vgq+hL7/v3q/9cBwwOzBZ8Hjwl/C28NYw9TEUcTOxUvFyMZGxsPHQce/yD3IvMk6ybnKOMq3yzbL
      tsw1zLXNNc21zjbOts83z7jQOdC60TzRvtI/0sHTRNPG1EnUy9VO1dHWVdbY11zX4Nhk2OjZbNnx
      2nba+9uA3AXcit0Q3ZbeHN6i3ynfr+A24L3hROHM4lPi2+Nj4+vkc+T85YTmDeaW5x/nqegy6LxU
      6Ubp0Opb6uXrcOv77IbtEe2c7ijutO9A78zwWPDl8XLx//KM8xnzp/Q09ML1UPXe9m32+/eK+Bn4
      qPk4+cf6V/rn+3f8B/yY/Sn9uv5L/tz/bf//ACwAAAAAIAAgAAAI/gBVCRxIEASfO3f4KOQz6A8c
      ghAjQhxkKtWpi6dSpaqU6FIkDRJDChyUqlQpUidLneqDJA8fSZUsiIwY4qKpmzdTNZJz6FChP5cG
      zSRIYZLGoxpNcYATJw6eQ4g4cZhpARGkSEiRmmoUyU4FOoUIUcoztE7Ws6kO2cgRBIjbIDp0vBCZ
      B61GTR8mqHARwwXfGDJkhKkiEtHRjKlQ+ZFgYMGCBpAhM1CgwMgjHiEJQEJ6ycOBAgYQOIDwgPRo
      CA6OOOoi8kKlVKb6YKBxQwgRJmLOpEmjqHdvRmSgoIhYp6ecRHI6ELkCJgyZMm+iS3/D5k0ZVSlM
      EIxzqNIlTZDq/mgaBUjJlS5hxnz54oVLlixTpviwMpzg8UmJIE3aJIoUKlSHLKFFFwRusYUVUkTx
      xIJVpAARIpcgkt8lnoxCyk2kzDECCSV06OGHIpxAUB2bTHLVeCiVMsomo1gygwoqrCDjjDK6UMKD
      o1xyySagTIJIhZdkssknSbyww5FIIhlECwTBMQkpn3xCyiQCIZJJJpVoAsoeT0DhAxRggunDFFTA
      YN8lppCC0iEDBWlJJp4AAkV88VFB5xRc4ACRJLDdtAmbg3hSCUyb6EEmFYgmiugYOkB0SGKpkBLK
      Jpt0cslVlVjixHtcdOqpe2NgZl8psAkqSSSJABKIIYjk0cQatmqMIesYZZRhRhtomDmRRpDAccgg
      gAgCyCCDBGJDBCzUUMMNPwxxxBEq1BcRIqeM4oddfgQgRyCBHJLIJKAYMlQgm/yhiSkpoWRKJx5U
      4h0mnIRSykNDqQLCHaeI0h8p+pYySCOl6CuKKZDUS5AkpFB6ZaWg2NEIKKCIAsoGBg9EgSSdYKIx
      JpVwwskfi4jSSQgVE5SAHYgsskgjjawsSSN/XFByRAEMYPMAAdQ8wMw89+xzRAEBADs=
    }
  }
  if { [ lsearch [ image names ] database_add_32 ] == -1 } {
    image create photo database_add_32 -data {
      R0lGODlhIAAgAOfXADpyADx0AD52AEV7ADmAADqBADuCAEKAADyDAD6FAUaEAmtvckeFBEKHAEOI
      AESJAGtzekaLAEmOAEqPAFuJM0+SAFCTAFOWAFmRIliUDX6Dhl6aF36FjWGYKn+GjnyIiIGIkIKJ
      kX2MkoSMk4WNlG2eOYOPkIaOlYePlm6hQomRmIqSpouTmoaVm4yUm4eWnI2VnGmwBGSyAHimT4ua
      oW6zAJGZoZKaom+1AHazAJCbqJObo5ScpHG3AGu5AHK4AJWdpZibt3m3AHO5AJaeppCgpnS6ApGh
      p5igqHe7AHa7BZmhqXi8AJOjqZqiqnm9AJSkqnq+AJujq368DIDEAKGqsYPGAKKrs4rFAKCsuYTH
      AKOstIXIAKGtuofFHKSttYfJAKKuvKauto3IAKevt47JAInLAKiwuJLBU5DLAKiwxYvNAJLII6qy
      upPJJauzu5DMGZvDV6y0vJLNG6m5oK21vZvFXpHSAK+3v6S+qZ7IYbK7w7S9xZ3TMbW+xra/x6XT
      M7jAyLXBz7nBybrCyrvDy6nVWLzEzK/UWLfJtb3Fzb7Gz8XE3L/H0L/G3MDJ0cHI3sTI2MHK0rTa
      ZcLL08DM2sPM1MTN1brbbsbO1sfP18TQ3sjQ2MfP5cnR2srS28vT3M/U18/T48zV3cHhiM3U6sXg
      j83W3s7X38Tji9LW5sjikc/Y4M3Z59DZ4dDY7c7a6MjkmdLa4s/b6tPb5NTc5cvnndTc8tva8tXd
      5tzb89be59fg6Nvf79jh6dni6trj69vk7N3l7t7m79/n8ODp8eTp7OHq8uXq7eLr8+Ps9Oft7+nu
      8Ojw+evw8+zx9Orz++7z9vP4+/T5/PX6/fb7/v7//P//////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAgACAAAAj+AP8JFFgECZIlCBMqNFhkoMOH/4hAEROI06hixDJqzFhs
      FKdAYpYQgTgQiJhRvnblksWypUuWuXb5kiUGCMl/TU6dGgWqp8+fQEGN0tnkZhFhlixlWsq0qdNM
      SYU1JAmkGrBHhQ4dUsS1q1ethR4BqybFqKJo1KD9kqWJktu3bjXJ+gWNmjNFUyEe8ROIkqxjzKZN
      k0a4sGBmxmRZChToyE0igQgVUvSIUiZOqGTFlIVqlCZLjxQdIjRoJNVBhLha5nSKlaxdvFie4vQ5
      dCFCNkmKEDO58uXWr2PLml1btJwQN0dUETNoNajMsnj5WokKFKdMlBThETPi5o1kZHj+OGmzRxEl
      VLt+AYudKXSdKkvEIENykwalZ8dCEXpTpb+YM2d0kUV/chQyijHNUEJDfYxlkssyz1RDzYSCTUhN
      Nc0ss0smgQiiQ32obfWIJZo8p9kunHmUSSWRQOIII0HcZAMhqZn3m2uwycbJJp3Q0QECCCQQgQMQ
      cUCGiKsBlyNnopQygwRomGKLLavYkQEDDnmwXHOXgXIKS9KtJMsrKVywiiFTxBDDFIjEsgGWAsGg
      THjjledXeuvJokoeEpDiRQ84WGMNDj14kUoGA71gCX768ecfgF10oQYGcbiRRBKBDnqpG3ooIFAL
      DT4YoYUVVmMNAZNE0UMPMggqw6rDUWASgUAuhEgZiSZu1gouBfSRhKDAApsEILP+wwKNqt0YnCyw
      6FIAHFEEG2wUcBTbwiCT2cgajrE1WwAVVEQRhQ+C+iAuFWMUq8EOg0ji3Jey+PJLLrPcUkIMYIAR
      rqBRUJFvDgcMBIEKO1TBRyGu8QLMMMQE80sviUgwhhn6CuqvGWVcQNICH4CAwgkmmEDCyCegsAIF
      F6SxRr75rpHGBQLcJLNDAVQgxBp33LGGEBUEMPPPAw0QwdARDAD00UgnnXRAADs=
    }
  }
  if { [ lsearch [ image names ] system_upgrade_32 ] == -1 } {
    image create photo system_upgrade_32 -data {
      R0lGODlhIAAgAOf/ACwsNDg2Oi09LSRGHUo7V0BERkNFQh9hETlXM2BdYS17GkB5MGdqeGlqc3Zn
      jCmJHm9tcTmGJHBtfEuBRXNwf3FyeyKWFAegEnp2hnx2gVOJTECTKSqbGxWkGACwBQ6rE0WYLiuk
      GkCdMQC1AGOOX4SChgC3AA+zAIKDjAC4AFCbOR+wBwC7AxaxJoWGj4iGihC2Ehi2AGOXZTmpM1ue
      PSKzHjCtNoyImTGxHUWnSQm/DIiNkB67GpCNnXiaeCi+DSO+HpGSnEW2JFWtTzy5JyPBLJSWkzq+
      EC3DJZmZo0jAJC7HM3KuZz7EJznFO52eqHG0UT/GNKGeojzHPUPHKpygr6Cgt0zHNZ+kpqOiuVvH
      LqSkulTKMKWnpIy0iU3NS1PNO6envqqorFLORZeykKaquVnMTGPKQ6mpv1/NPV3ORq6ssIW/cVzP
      T2HNVa+tsaquvq2vrK2ut5+0pmjOR62txF7TObGuv1/SUZS8kbKwtLCwumjRUZm8mWzSTLK0sbOz
      vbayw7ezxWzVVbW1v7i1unTWXnnWULq3yG3ZYLW5ybi6t3zVX7m5w6DEoHvWZnXZaHrZW7i8zKzC
      tL68wLi+wLq+znrcZIDaa8G9z3zeZobacofcX73CxL3B0cbAy4XeZ8LCzL/D0oXfb8bDyMPFwrnM
      uIficoziZIria8bH0ZDjc5XheorldMjJ08vJzZjidLvRw5PldtHK1p7lcZzld5boeM3O2NDO0sjR
      zMvQ0pjnhbzXwdDR26LpidbS5KLsftnS3tTU387Y09jW2rnnn6Pwh9Xa3KfxgtzZ3tna5LHyhdvc
      5rjupdze27Pykt/c7t3e6ODe4rH1juHf49bl2MLwqLX3ieDh6+Ph5eTh8+jh7d3m4ebj6MfyuN7o
      4r/4oOXm8L77lOnm69D0u+ro7OXq7ejp8+vp7e3q7+Tu6err9u/s8dD9re3v6/Du8vLv9O/x7unz
      7vTx9u7z9vHz8Pbz+Pj1+vz1//n3+/j69/37//j9//v9+v3//P///yH+EUNyZWF0ZWQgd2l0aCBH
      SU1QACH5BAEKAP8ALAAAAAAgACAAAAj+AP8JHEiwoMGDCBP+A+TPHyCFEAXiIvik4ROCEyMOLGTP
      X6OBSRpiGZjJX75CGv9VaMgPAqVt6jbx2WWNXDNWjLw5SPnvVj9++/w9SuPrGbJly66Ja1dLC8Fi
      ClHwy1ePyKhTqU7JkmULGLCj4CIJhEWroBxKAsXks3cvBKRBhhJd0nS11VZbtozBkjUNVcFu9saJ
      sQdvnwg3Y9S0WYwkRQoWURJd1cqVE0FA99i9g8cOnqMZUZxM+QLEhIcLHVawiEEF06VLoEaJJeji
      XDp06PApWFKkiBPHDw4MEIAgApEfTRgpZ3TIIIB048rFssCDBxAeKTgQDEBgQ5MjV/j+iPdjMMi4
      budk1IABg0eMGgYXcFFyxYx9M2cMAsJGbduEGh7EEIMJOBhkRxp00KFGGlQcocRAr5BCSS/MMBOM
      Bi14YIIJLBwkhAggqCAiDVCwMYtAmdyiTDQVBkOCDQPG0OFBGfTwyS/QaAPNL54IhMYWWWxRRyCu
      +DCECRBJsYcqgdwRRpBoCIQBBRJY8aQkczDxAURydNJIFmFYIQEFGBR0w49oUONFDgoZ4YozZWyB
      xg0IMVAHGmikM0keClUijDlloFEHAwgl0GQgntDDSx8IURKOPZ4cmsBBhCByhyWKSMKMOtWYQgZB
      ceCSzjzKSKKIJXcgQkhBVaBxRyCyosQaCjbycMPNMLnockw65dyTTSixitIkGlUUlCocDQijyi0q
      xlOPOdCyY489yjCrijANwGGpQQ00IFAF0fQiTDLJUGNOPPCkQw25xPQSTQUCdQtRCdRkI0023Yyj
      73n2ZkNNCTz9Q0y588DjzsEIwzPPusTwtEa57xiwQynu1FOPO6XsUMA7676R0gvJCDZQFxZ3MZAY
      5STzQsB6EPRHPPH8QVDLARu0iD76LFLzzhAFBAA7
    }
  }

  if { [ lsearch [ image names ] pck_ok_16 ] == -1 } {
    image create photo pck_ok_16 -data {
      R0lGODlhEAAQAMZBAAB6AAB7AAB8AAB9AAB+AAB/AACAAAOBAgWCAgmFBQqFBQuGBgyGBg6ICA+I
      CBOKChWLCxaMDBiNDRuPDx2PDx2PECOTEySUFCiVFSqXFzyhIT6jI0CjI0moKEyqKk6qKlKtLVew
      MF60NGa3N2u7O227O267O3S/P3XAP3bAQHfBQXrCQn/FRYDGRoXISITJSYrLS4zNTY3NTZDPT5HP
      T5LPT5LQUJPQUJPRUZTRUZXRUZbSUpfSUpjTU5nTU5nUVJrUVP//////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////yH5
      BAEKAH8ALAAAAAAQABAAAAemgH+CggQCAQIDg4qCBQkaKjItIBEEi38FFzs2mzY0OiIGipidnDQ2
      MzQllYIKO6acNiYIMTcYBX8EG7CdKAQsnS4BfwIrsJ4HH6RAC38BNbA4HBE+nDwTwy82LCQqMwAp
      rzY9D38DITQ0EQEVFjywPIl/Ejk0Ow4DJOE2HasEI6Yz8sHagUCRgROkBjKwRCADjB87evTwUNDS
      pQANKEAgsMqiR0WBAAA7
    }
  }

  if { [ lsearch [ image names ] pck_new_16 ] == -1 } {
    image create photo pck_new_16 -data {
      R0lGODlhEAAQAKU9ACVSiixYjS1YjS1Zji5Zji9aji9ajzBbjjFcjzJcjzJckDJdkDNdkDRdkDRe
      kTVekTVfkTZfkTdgkjlikz5llUVqmEhtmUlumkpumlZ3n1t7oWSCpmqGqHKMrHONrHmSr4KYs4eb
      tY6huJurvpyrvq65xrS9ybjByrzDzMDGzsbL0crN0szP09DS1dHT1dXW19bW2NbX19jY2NnZ2dra
      2dvb2tzb2t3b2t3c2t7d29/d2+De3OXi3v///////////yH5BAEKAD8ALAAAAAAQABAAAAahwJ9Q
      2CgMCIihUgiphFKwFUnDWP4iJt1sRpvJcCaJ0mGiybyis9cUET44u+3sBXjJZ5zHb+FSz+t3LgU/
      BXFydHZyOxY/BIZbiHc7GYRxIxEtiC0RIzM7Fz8KfTIdDiwALA4dMjIxgz8YOVwfFA8UH10zHg5C
      DipeNiABIDasKmJDESo6MjYoxTUnE1YOGyU3PDclGwlWQhIJBAYCB97m3kEAOw==
    }
  }

  if { [ lsearch [ image names ] pck_upg_16 ] == -1 } {
    image create photo pck_upg_16 -data {
      R0lGODlhEAAQAKUpAJAzN5A0N5Q2NZU3NKNALaRBLOJmAONpBeNpBuNqB+NqCONrCONrCeRrCeRs
      CuZtCOlvBuRxE+xxBe9zA+d9JuiCL+iFNOmFNOqPReqQRuqQR+qRR+qSSfO+lPO/lfO/lvbPr/ja
      wvjdx/ngy/niz/rl1Pzt4/zy6v79/P//////////////////////////////////////////////
      /////////////////////////////////////////////yH5BAEKAD8ALAAAAAAQABAAAAZ3wAFg
      SCwSBT/ABMJsOiGSwi8AcVivWMeDMK1eFRYKdtvFGkafMZdqHnnUZQeDcX7PtesqoxKpMy5WZGwO
      JigIIx0YKRsMgnoNJSckIikcDHhxDgkhKSkaC4F5WAggGQpXjlkHl6hcSk+wUT9CRrUASD+5uru8
      P0EAOw==
    }
  }

  if { [ lsearch [ image names ] pck_upd_16 ] == -1 } {
    image create photo pck_upd_16 -data {
      R0lGODlhEAAQAKUzALgAALsAAL0AAL4AAL8AAMAAAMEEAMIHAMQNAMQOAMYTAMcVAMshAMsiAMwl
      AM4qAM8vAM8wANM7ANZGANdGANhLANtTANtUAN1ZAN1aAN5eAOBkAOFlAOJpAO2KAPGXAPKaAPqw
      APqxAPqyAPu1AP6+AP6/AP/AAP/BAP/CAP/EAP/FAP/GAP/IAP/MAP/OAP/PAP/TAP/UAP//////
      /////////////////////////////////////////////yH5BAEKAD8ALAAAAAAQABAAAAZswJ9w
      SCwaj8hhIVksIAbMoWC0iP4ImRUIyhyoIi/JEknwwAAvEfd4QL3QqA7hWAid3q+TaUwkPDgQMgAx
      DRwfc0QGJSkDFwEUAykoVUMDGHoqAwFeeiSIPwovLKMuAy6jLC0TcwOtrq+wVkVBADs=
    }
  }

  if { [ lsearch [ image names ] pck_upg_upd_16 ] == -1 } {
    image create photo pck_upg_upd_16 -data {
      R0lGODlhEAAQAKU2ALgAALsAAL0AAL4AAL8AAMAAAMEEAMIHAMQNAMQOAMYTAMcVAMshAMsiAMwl
      AM4qAJU3Nc8vAM8wANM7ANZGANdGANhLANtTANtUAN1ZAN1aAN5eAOBkAOFlAOJpAORsCuVvCu2K
      APGXAPKaAPqwAPqxAPqyAPu1AP6+AP6/AP/AAP/BAP/CAP/EAP/FAP/GAP/IAP/MAP/OAP/PAP/T
      AP/UAP///////////////////////////////////////yH5BAEKAD8ALAAAAAAQABAAAAZ9wIFw
      SCz+Bp+kcqkcHJmfAtSJXBYQ1eZzKTAtmNQlQeMaZT/hJGjQGsgm0mT6QwjN3KXs/LCSuVceBHJb
      BSQqfjIqKXFhBA8dETUDNA0dIoJhBigsAxgDFQMsK19UAxmKbUItiicEVAoyL7IxAzGyLzAUpUW8
      Qj+/wMHCv0EAOw==
    }
  }

  if { [ lsearch [ image names ] clowd_down_16 ] == -1 } {
    image create photo clowd_down_16 -data {
      R0lGODlhEAAQAKU1AABVmABVmQBWmQBrqwBtrGpqamtrawCLxQCMxgCPyHd3d3h4eHx8fACu3gCw
      35GRkQC14ZWVlZeXl5mZmQDN82S53aqqqqurq2q63aysrK6urq+vr7Ozs7S0tLa2tmDL7mHL7bi4
      uLu7u8HBwWnW98TExMXFxcbGxszMzM3NzdXV1d3d3eLi4uPj4+jo6Onp6evr6+/v7/Hx8fPz8/T0
      9P///////////////////////////////////////////yH+EUNyZWF0ZWQgd2l0aCBHSU1QACH5
      BAEKAD8ALAAAAAAQABAAAAZ1wJ9wSCwaj8ik8ldg0RbIwqKgmLVasJBx8lrJZKvwylUoxlRogCCA
      VkmIhVZqPiAM5qnHsLAq+UsHCQh/JShCIyKJIhgNEA4ViiZlKRyVHB0gFB8dlh4GPwwZF6MXGiQa
      pBcnQxEbFq+wsCdlewa2t7dLurpBADs=
    }
  }

  if { [ lsearch [ image names ] star_blue_16 ] == -1 } {
    image create photo star_blue_16 -data {
      R0lGODlhEAAQAPZLAA1piA5xkg52mA94mw97ng99ohB/pCuAnBCBpxKCpxWCphCCqBKDqBWDqBKE
      qRCHrhKHrhiEqBqGqRuJrRyIrBOLsxGMtRePtx2LsByMsRGQuhGRuxKRuhSSuxSSvB+SuB+WvCyF
      oiyLqiKPsyORtCqUtyKUuiuXuzCavDCcvzKcv2ehtCSZwC+fwy+fxDigwjuhwj6jxDqkxzylxz6l
      xz2lyEGlxkCnyEKnyEKoyU2tzGivx2mxyGq61HrB2Ya0w5W9y4jG2pbI2bPQ2pfP4aXW5cPh68Pj
      7tHj6eHs8OHx9v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAEwALAAAAAAQABAAAAetgEyCgjM0NjEvKoOLTDU3LB4bGx0gKCeMOS2Sm5smJYM1mps6Opwf
      JII3nBtHRg+cI0wyLJwuS0shnBcZOJE6PkRKt0lBPqUcFDaSPbfNzTuvEjGbPM63KwALGxEwHZtC
      zkDZGxoKKSCbR85DBJIVDkwom8I/QEtIApIJgicmkkUHABQQMSTABggIBpX4IGmABUkWBEAwwIjE
      iAscJGmokCAhI0EYJkiI0IABo0AAOw==
    }
  }

  if { [ lsearch [ image names ] package_down_16 ] == -1 } {
    image create photo package_down_16 -data {
      R0lGODlhEAAQAPcAADJwKjZ6MUFqIkNsJ0ZvJkBvK096LVyAPUePQEuPQUqMRUuNRk2PSFeNQ1CS
      S1WaTleZUVejTlikT2a2XGi4XWytZXysZ22+Yny/dnDCZXLFZnPGZ3THaH7AecODIsOEI8CFJcSF
      JMWHJsaKKMeNK8iPLcmQLsmSL8iVMsuVMsyYNMiUO86dOd6fMtGkP+KqN+KpOOOrOua0Pea1P+y2
      OOm1PcqQStCXU9KlQNOoQtSqRNWsRdWtR9awSdewSteyS9iyS9i0TNi0Tdm1Tdm1Ttm3T9WmUdqt
      UNuxVuGnQOa1QuaxR+i+Q+m9ReSxTOi/SOi7T/W9QOWvVtuyYt25ada4cNy9d+a2a+W/b+SzdIDB
      eoLCfYXDfuzFR/nESOHAcuDAfeDGfvXPf//Vd/XQePXQf/7WeIfFgZHKiZPLjJ3QlaDSl6nXoKvY
      otvDidfTj97JlezFg+7Eg+3Gh+zKhOvMhOfMiO7KiPHPhvvchPfViffXi/DQjOXPku3SkurVnPHV
      kPXXkPLWlf7flfjbnf/hm+3ZpfLdr+3evv/kpfnhqvrjqv/oq/fksvrksP/qs//ptv/rt/blv//q
      uf/ruv/rvf/tvv/uwf/vwfvty//vyPnrzfrszv/wyf/yzvXr0vfu0f/z1P/z1f/01//12f/02v/2
      2//23fz24v324v724/744f344vz45v775//+8P//9gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAALEALAAAAAAQABAA
      AAjnAGMJHBgLiA+CCAcK+fHHzg4dCQcG6dNK1aFGX1wkJBIGFKxXmY7sUSRoCguBRYYY+oQIjpsq
      VAKJIbSIjpEUQlZt8lMHC5IVnEhNGjQmUZ4TP1iFcoSnDBknjiAVMhOlBqASPVKVElVJD4cNGS54
      ofEECh8SPFCN8qSJkoY2bCh0aaJkyZ0ROlCF6mTp0YQ1aiQwmREjSRwROVydwhSJUYQ0aBDIgNHi
      ipwQOKxIMnXpzYMzXAK8kDIniw0PAlmAsZCgwhYtDgAcuPEBoQoUDSB0wLCgAIjaEU0YYKBgQIiI
      CAkIiBgQADs=
    }
  }
  if { [ lsearch [ image names ] system_software_update_16 ] == -1 } {
    image create photo system_software_update_16 -data {
      R0lGODlhEAAQAPcAANpuFd5uFtVwFd1xFedyFedyFul7Fel8FuBxGOd2H+Z6G+l+G99yJ9x/IOBz
      JeZ6Ieh7JOJ3LeJ4K+h/Leh7Meh7Muh8Mul+P+R/QOh/QcmBGM2GGd+AGt2LHdiRG9qTHN+YHd+a
      HuWAH+CPHeOMH+iHJuyMJ+uOK+qdKOiCPOiFPuqOOuuOO+2WPeycPe2hLO+kP/CtPueGR+mCROmL
      SuqMS+mMTOuOT+OZQeqRSOuSS+qQTuyZTueLU+mQVeqRVeybVeuTWO6qTvCwQvK3T/K4UuuQYeiS
      YOuSY+uUYeqYZ+ued++oYOuheu6meeuhfO6kfvC1cvG3ffPCeu2mgO6ngO+uhe6ujfK9hfC1jO2s
      kO+ukO2uku+xlO6xl+6ymPG6kPK+lfK/lvC4nvC5oPK+ovK/ovG9pfG9pvG9p/XJiPPAkPXJnPbO
      nfXMoPTGq/LDrvTKr/TLsfjetfbWufbUuvbUvPbWvPbVvvfavfjevfnhu/bVxfbYwvjexPbXyPbW
      yfbXyffZyPbZzffe0frmyfrny/jj1fji1vji1/nm1vjg2Pjh2fjl2vnk2/rp2frr2frs2/no4fvu
      5Pz08AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAJUALAAAAAAQABAA
      AAjJACsJHEiwoMFKWrx84XJw4JItY9CkIdPlycEjVM4AWsRoEJwrTQpiUFLGUaM/fBJJCmSmB0EZ
      VhAJqoLECBRClOT4GMjARpxDTmZcyJDkjZ0sNCIIdLCjD54bFihUSFEjyA8VEgQGeJBDx4QEBQgQ
      gACmTg4EAgEoABJGDI8FBg6wuENnxYCBIqRMUsTkhIkWax5hKSFgYIMoeiL5ccMmD6Q2LjgUxCFk
      ypxChvaogUHiYAcUMYgUGfJiRMNKGjyACPFhw+nXAgMCADs=
    }
  }
  if { [ lsearch [ image names ] check_green_16 ] == -1 } {
    image create photo check_green_16 -data {
      R0lGODlhEAAQAPYAAACVAACWAACXAACYAACZAAKaAgOaAwecBwidCAmdCA6gDQ+gDhCgDxOgERWj
      ExymGiSlISirJSqsJCqsJi+uKzCvLDKvLzayMTeyMjixNDy1Nz21OD61OT+1O0C2O0O1P0O4PUS4
      PkW4P0m2REe5Qki7Q0q7REu7RlC+SlG6TFK6TF3CV17EWGjLWm3FZmnJYm7JZ3LMZnnNb3TQbHjU
      a3vPcHrTcoDTdYDUeYXWeYbQfoXUfYfeeojeeovaf43VhY/VhY7WhonYgozdgI/dhJDXh5Peh5HV
      iJHWiZLWiZLaiZPaiZbdjJTfjJnbkJ3elJHmhJTkh5rljpnikJ/ml57rkZ/qkqDgl6HgmKHimKbl
      nKjmn6XsmajqnanvnanmoKnooKzroazoo6zuoK7qpbHtp7LsqLPtqbLuqbfzrLnzr7zzsr/1tQAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAG0ALAAAAAAQABAAAAeZgG2Cg4SCBQgLhYptBSouKQ+LgwYfRzJIEpKCDD8xMUUTmgkwSy1P
      KxSaFlc0TDsVmgtKRkNZHoIAECMNBJssWj1kLxeCATpAQRYHbRFiUWFNGIMCSTU3VyQOQl1WZSWE
      AxlOOT5bOGdVazMahQUdWEQ8ZlxpVBuLCidfUlBeaiiaHmwAM4aNDRCa2kwwgWZKiISCMHAQISkQ
      ADs=
    }
  }
  if { [ lsearch [ image names ] cdr_tick_16 ] == -1 } {
    image create photo cdr_tick_16 -data {
      R0lGODlhEAAQAPZ2ADd7MTh8Mph/Mpl/Mpp/MjqUMECFOWuPPUijPFWcTVKtQ1ymUlyjVGuqXGC1
      UGi3XmyyYnS5ZZmAMo2vZo6waY+ya5Czb821b861cM63cs+3cs+3c9C4ddC4dtC5dtC5d9G6edK8
      fdO9f4HKdoPJeILLeIbJeYXLeYXIeoXJeofMe4bMfIfNfInNfYnOfovPf9S+gNS/gtW/g43PgZDS
      hK/LlcDAhdbAhdnGjtnGj9nGkNrHkdvJldzKltzKl93Mmt7NnN7NneTWruTXsObZs+batebatufb
      t+fbuOfduujduujdvOnevtPhvurgwOvhwuvixezixOzixezjx+3jyO3kyu7lzO7mzO/mze/mzu/n
      z/Dp1PHq1fHr1/Lr1vLr1/Lr2PTv3vXv3/Xw4fXx4vby5Pfz5vj16vn27Pn27fn37fv48fz69Pz6
      9fz69vz79v379/37+P78+v79+/7+/P///v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAHcALAAAAAAQABAAAAfMgHeCg4SFhjxOXGRgPgSOhR9zYjg/WHN1cx+PdwQfa3ZwITJedm5v
      dpqDXzdbdj0ZY3NncnRzjgQ+QSAdUFlDYWpncWxtjQRMMCAePSKOMW1lXVJMjlEcHyBFtwRXQDoU
      DQcEURsYF0fbTzsTECgJBEsaFzlbMY4iRBYQMy8GuEZWzIRp8qAGkgoQWrAIQEDQnDRfpkQwscCG
      gxUnGDbshEaLEgU0UkAgUSLABwmEPoyhIgSBCxUjTG4ixCNJlQIMGAowRGgAAQANCQUCADs=
    }
  }
  if { [ lsearch [ image names ] database_1_16 ] == -1 } {
    image create photo database_1_16 -data {
      R0lGODlhEAAQAPeLAEdHR0dHSEtMTEtMTU1OTlNUVFRVVVZYWFdYWGFjY2JjY2JkZGNkZGVlZWRn
      Z2VnZ2hoaGhpaWttbW5vb29xcXN1dXV3d3Z2eHh5eXl5eXh6enl6enp6en19f3x+fn6AgICCgoGD
      g4mLi4qKjI6Ojo6OkJGRkZGRk5OUlJSUlZSUlpSVlpWWlpaXl5iXl5aXmJeYmJeYmZmYmJmampqb
      m5ydnZycn5+fn6CgoKCgoqGioqCjo6Smpqeoqautraytrq2urq2vr66vr66wr62vsLCxsbCysrGy
      s7Gzs7Kzs7GztLKztbK0tbO1trS1tbW1tbS1trW2trW2t7W3t7e3ura4uLe4uLe6uri4ubm7u7q7
      u7u7vru8vr29vb6/wb6/wr/AwMLCwsPDxMXFxcXGxsXGx8bGxsXFyMbHycjIycjJysvKysvLy8rL
      zcvLzcvNzcvMzs3Nz8/Ozs7Oz8/Pz8/Q0tHR0tLS1NfX19bX2Nvb29vc3Nzc3Nzd3d3e3t/f39/g
      4Ofn5/Dx8fX19vj4+fn5+fz8+/v8/Pz9/P39/f7+/v///wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAIwALAAAAAAQABAA
      AAjOABkJHEiwYEEyRnYg0fPnzRWDc+7YiaOmjBUtZNjIWUOQyZYveQQFamFBBI8xWQhCoYJmkKE9
      QT4kqFBkCsEmXNoQStTHx4cFGKI4IRjjgoo0fMzMmICAQQYZBInYGJGiBokGAgpMuHGEoJQtbggh
      2gMkxIMNTqoQVLIFDiFFfoD8xPAkCcEXF06IwROGhYQDCji4ILgkRwkYOExAIGCAgg6bA3t4aVOI
      pxCzGqz8ILjiTJ1DiwANAeHAAxgaBQN0wEKnC4oIAwQAMEjbYEAAOw==
    }
  }
  if { [ lsearch [ image names ] back_to_ou_16 ] == -1 } {
    image create photo back_to_ou_16 -data {
      R0lGODlhEAAQAPYAALRmJ33VJtuPGd+aHeGhHt+bL+WwKuaxPOe4PYnVPu3UAO3VE+3QF+7VHPDb
      KvDYL/HdN8+kU+m9Q82nYt62Ztq1aeewY+K1dY3cQJDXSpXdUZvdW4jCZZHJaJbObu7PR+zJS/Hd
      QezHU/DYVPLgTPPiWfPjWfPfZmZ3mHiGq3aIsH+Pt6u816Kx3LPA4b/P4r3F9L3G9s7Pzs/Pz8/S
      18/T19LX3dLY39jY2Nvb29zc3N3d3cHM48vZ68vb79LY4Nba4NPV8dPZ+ePj4+np6ert/PHx8QAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAEcALAAAAAAQABAAAAd8gEeCg4SFhR8RLIoshoIDBw8MFT0+Pi+GCCQQDgoNF0VFPIUEJiUg
      IQsUQUJCLoUSJwJHIxMwMbcthQYiggUqKykrKyiGFo3HRwDIy8xAPz80RzY3NzVHHhwcHYJERkY5
      R0PeOkcbGBgagjtDQzNHOOwyRxkBAQnM+Pn5gQA7
    }
  }

  if { [ lsearch [ image names ] folder_yellow_16 ] == -1 } {
    image create photo folder_yellow_16 -data {
      R0lGODlhEAAQAPYAAK+KGLGMGLOOGbKNGrKMG7OMG7OOG7SOGbSPGbWPH7SQGaOFI8GLNMmJN8Se
      MMSXOcSfO9eQOuOePN6xLcOhN8WgNMahNcehNcmjNMijN8mmNcukN8ulN8umNsumN8KgOMeiOMei
      OcemPMekPselPsemPtGnN9uqNeG9K+K4LeO/LOS8LeW/LeytO/G5OeTALPTHN/XMNvTBOffQNcun
      QtGtQtCuSd6+Q922Sum8S/TCTvjETvjITvjOTe/USO7USfrTTfzaS/veTPzgTPzkTfzmUfvpXd3A
      ZurTg/rrhvLgl/nrqAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAEwALAAAAAAQABAAAAeSgEyCg4SFhoeGBwgICgGIggYTKSssAgqXlwiEAC4yMDEzKi8vKioo
      A4MALScYGxwesLAdFRcgTAASJj9GSUu+v0tKFkwEERlCREW8wEtIIUwFDTVAQUPJy75HggUMODw9
      1NbKvTbbDzk7O9/h1w7bEDrp6evVPhqCCTTy++s3gwsfSJQoQWIEiYIjRFB4xLAhoUAAOw==
    }
  }
  if { [ lsearch [ image names ] software_update_urgent_16 ] == -1 } {
    image create photo software_update_urgent_16 -data {
      R0lGODlhEAAQAPYAAHgXF6wiIqUtLa8vL7AjI+EXF+AZGeIaGuUdHdQoKNUpKdYrK9ctLdcvL8ox
      Mco5OdgxMdg1Ndg2Ntk/P+giIu4nJ+gqKu4oKO4qKustLewxMe8wMO8zM+80NO44OO84OOk+PvA6
      OvA7O/A8PPA+PsFCQshHR9hTU95YWOpHR+5FRe9FRetISOxKSvBCQvBDQ/BERPFGRvFMTOtSUuVe
      Xu9YWPBQUPBTU/JWVvNbW/JcXPNcXPJeXvNeXulhYfR8fPV+fvR/f+mAgOqFhfWAgPjR0fjV1fzY
      2Pje3vzf3/zn5/rq6vzu7v309P729v34+AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5
      BAEAAFAALAAAAAAQABAAAAdzgFCCg4SFhoeIEIIFBoiDDRUHT08WUAmIkAhLTBoVCokXFEhKHRUL
      iBIbGUZJHBgMhg8TKSEeRUcfHCARDoUzMC8uQUQkIyIshgE1MipCQysxNgSHAjwtTU43OAOOJT0/
      QDomjoInOzko5IM0PuqEAO7qgQA7
    }
  }
  if { [ lsearch [ image names ] view_refresh_16 ] == -1 } {
    image create photo view_refresh_16 -data {
      R0lGODlhEAAQAPeiAChAYClBYipDZSxIbixIby9LcS9Mcy9MdC9NdS9NdjBMcjBMczZSdzZSeDZS
      eTZTeTdTeTdTejdUelBtlVBtllFvl1BvmFFvmFJvmFRxmFVymSxtxCltyC1wyyxwzDJxxjZzxzZ0
      xjZ1yz56yzl5zj580EB+0GqJs2uJs2uLtmyLt22MuHCPuHKQunWTu0eBzkiAzEyCzE6DzEyDzk+F
      zU+Fz0mD0U2H00yG1E+I1FGI1FGJ1FOL1VSL1FSL1VWM1VuN0FmO11mP11qP11yQ11+S2GaV1WuY
      1GCT2GOV2WSW2WWW2WqZ2Wqa22yb222c23Ce3Hih13Wg2nSh3XWh3Xai3nih2Xih2n6l2Xmj3Xij
      3nmk3nmk332n34ekzYqnzoem0Iin0Iin0Y6q0Yqv35Ov1ZWx1p643J+53YCp4Ias4Yiu4o2x446z
      45a24Ze45p664J+74Zi55py75J6+55++56O94KO94aC84qK+46e/4KC+5qS/5KTA5afB5KjB4ajB
      463F5qTB6afE6ajE6avG6qvG667H6a3H667I67HI57LJ6LXN7bfO7rjP7r/U78DU78fX7cLV8Mja
      8cnb8svc8szc8tvn9uDq+OHr+OLr+Pf6/fj6/fr7/Pv8/v39/f7+/v7+/////wAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
      AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAKMALAAAAAAQABAA
      AAjyAEcJFKggwYIFCRQMXCgQQosyihaZcRGB4SgEGPy0odLkSZc3gTQgWFiBUBIfVBIN4rIEyqEM
      AyHkIXJDUihRojxNKsIEkINRClakKdRIFCU5dSqJEqSDDIuCYoZcEpXpyRQtUxA56mFkjEE+JiBx
      srSmiZMmStgwmfHnIJ8SPnDsEFIEiRpMoubA0IMwDA8Siejk4PEjiKZPkYB8Sagii4c1ohhV2fII
      lJsjWFAolBDHRgdDm3B26vNBBpoHAy/secFBhJQrIzbEwDNh4QELcKzUABGCRpQzFAwwFNAgBZg7
      drycYBDAosAABAoUGACAYUAAOw==
    }
  }
}

source [ file join $::TolPkgGUI::cwd tolpkg.tcl ]

proc ::TolPkgGUI::CmpVersion { v1 v2 } {
  foreach { v11 v12 } { {} {} } break
  foreach { n1 v11 v12 } [ split $v1 . ] break
  foreach { v21 v22 } { {} {} } break
  foreach { n2 v21 v22 } [ split $v2 . ] break
  set cmp [ string compare $n1 $n2 ]
  if { $cmp } {
    return $cmp
  } else {
    return [ expr { $v11 < $v21 ? -1 : ( $v11 > $v21 ? 1 : ( $v12 < $v22 ? -1 : ( $v12 > $v22 ? 1 : 0 ) ) ) } ]
  }
}

proc ::TolPkgGUI::lsearchi { i l it } {
  set _i 0
  foreach _it $l {
    if { $it eq [ lindex $_it $i ] } {
      return $_i
    }
  }
  return -1
}

proc ::TolPkgGUI::ReadPackSyncInfo { } {
  variable packSyncInfo

  array unset packSyncInfo
  foreach p [ ::TolPkg::GetPkgSyncInfo ] {
    array set packSyncInfo $p
  }
}

proc ::TolPkgGUI::ReadVersSyncInfo { } {
  variable versSyncInfo

  array unset versSyncInfo
  foreach p [ ::TolPkg::GetVersSyncInfo ] {
    array set versSyncInfo $p
  }
}

proc ::TolPkgGUI::ReadLocalVersionInfo { } {
  variable localVersionInfo

  array unset localVersionInfo
  foreach pv [ ::TolPkg::GetLocalPackages ] {
    array set localVersionInfo $pv
  }
}

proc ::TolPkgGUI::GetRepoId { url } {
  variable urlmap

  set url [ expr { [ string range $url end-13 end ] eq "repository.php" ?
                   $url : "${url}repository.php" } ]
  set repo [ expr { [ info exists urlmap($url) ] ? $urlmap($url) : "unknown" } ]  

  return $repo
}

proc ::TolPkgGUI::GetPkgNode { T url p } {
  variable urlmap
  variable treeParents
  variable nodesInfo
  variable packSyncInfo

  if { [ info exists treeParents($p) ] } {
    return treeParents($p)
  }
  set url [ expr { [ string range $url end-13 end ] eq "repository.php" ?
                   $url : "${url}repository.php" } ]
  set repo [ expr { [ info exists urlmap($url) ] ? $urlmap($url) : "unknown" } ]
  if { [ info exists packSyncInfo($p) ] } {
    array set packSync $packSyncInfo($p)
    set lastlocal $packSync(lastlocal)
    set lastremote $packSync(lastremote)
    if { $lastlocal eq "" } {
      set img "pck_new_16"
      set nodeStatus new
      set statusLabel [ mc "new (%s)" $lastremote ]
    } else {
      set idx [ CmpVersion $lastremote $lastlocal ]
      if { $idx <= 0 } {
        # could be -1 (because of date error)
        set img "pck_ok_16"
        set nodeStatus ok
        set statusLabel [ mc "ok" ]
      } else {
        puts "$lastremote > $lastlocal"
        set img "pck_upg_16"
        set nodeStatus upgrade
        set statusLabel [ mc "upgrade (%s)" $lastremote ]
      }
    }
  } else {
    set img ""
    set nodeStatus unknown
    set statusLabel [ mc "unknown" ]
  }
  set nid [ $T insert [ list [ list $img $p ] [ list $statusLabel ] \
                            [ list $repo ] ] -at end -relative root \
                -button auto ]
  set treeParents($p) $nid
  set nodesInfo($nid,type) pkg
  set nodesInfo($nid,status) $nodeStatus
  foreach st {new ok update upgrade unknown} {
    set nodesInfo($nid,children,$st) 0
  }
  lappend nodesInfo($nodeStatus) $treeParents($p)
}

proc ::TolPkgGUI::FillTreeInfo { T } {
  variable packSyncInfo
  variable versSyncInfo
  variable localVersionInfo
  variable treeParents
  variable nodesInfo
  variable localMostRecent

  array unset treeParents
  array unset nodesInfo
  ReadPackSyncInfo
  ReadVersSyncInfo
  ReadLocalVersionInfo

  $T configure -table no -filter yes -columns [list \
    [list {image text} -tags NAME -label [mc "Name"]] \
    [list {text} -tags STATUS -label [mc "Status"]] \
    [list {text} -tags REPO -label [mc "Repository"]] \
  ]

  # insert package information
  foreach p [ lsort [ array names packSyncInfo ] ] {
    array unset pkg
    array set pkg $packSyncInfo($p)
    set url $pkg(url)
    set pkgRepo [ expr { [ string range $url end-13 end ] eq "repository.php" ?
                         $url : "${url}repository.php" } ]
    GetPkgNode $T $pkgRepo $p
  }
  
  set localVersionNames [ lsort -decreasing -command ::TolPkgGUI::CmpVersion \
                              [ array names localVersionInfo ] ]

  array unset localMostRecent
  # insert version information
  foreach pv $localVersionNames {
    array unset pkg
    array set pkg $localVersionInfo($pv)
    set pkgRoot0 [ lindex [ split $pv . ] 0 ]
    set pkgRoot [ lindex [ split $pkgRoot0 # ] 0 ]
    set url $pkg(_.autodoc.url)
    set pkgRepo [ expr { [ string range $url end-13 end ] eq "repository.php" ?
                         $url : "${url}repository.php" } ]
    GetPkgNode $T $pkgRepo $pkgRoot 

    if { ![ info exists localMostRecent($pkgRoot) ] } {
      # will be used in contextual menu
      set localMostRecent($pkgRoot) $pv
    }
    if { [ info exists versSyncInfo($pv) ] } {
      array set versSync $versSyncInfo($pv)
      set dateremote $versSync(dateremote)
      set datelocal $versSync(datelocal)
      if { $datelocal eq "TheBegin" } {
        set img "pck_new_16"
        set nodeStatus new
        set statusLabel [ mc "new" ]
      } else {
        set idx [ string compare $dateremote $datelocal ]
        if { $idx <= 0 } {
          # could be -1 (because of date error)
          set img "pck_ok_16"
          set nodeStatus ok
          set statusLabel [ mc "ok" ]
        } else {
          puts "$pv OUTDATED : $dateremote > $datelocal"
          set img "pck_upd_16"
          set nodeStatus update
          set statusLabel [ mc "update" ]
        }
      }
    } else {
      set img ""
      set nodeStatus unknown
      set statusLabel [ mc "unknown" ]
    }
    set repo [ GetRepoId $pkgRepo ]
    set parent $treeParents($pkgRoot)
    set nid [ $T insert [ list [ list $img $pv ] [ list $statusLabel ] [ list $repo ] ] \
                  -at end -relative $parent -button auto ]
    set nodesInfo($nid,type) pkgver
    set nodesInfo($nid,status) $nodeStatus
    incr nodesInfo($parent,children,$nodeStatus)
    if { $nodeStatus eq "update" } {
      if { $nodesInfo($parent,status) ne "update" } {
        if { $nodesInfo($parent,status) eq "upgrade" } {
          set nodesInfo($parent,status) "upgupd"
          $T itemcolumn $parent NAME {image "pck_upg_upd_16"}
          set txtUpgrade [ lindex [ lindex [ $T itemcolumn $parent STATUS text ] 0 ] 1 ]
          $T itemcolumn $parent STATUS \
              [ list text [ mc "update/%s" $txtUpgrade ] ]
        } elseif { $nodesInfo($parent,status) ne "upgupd"} {
          set nodesInfo($parent,status) "update"
          $T itemcolumn $parent NAME {image "pck_upd_16"}
          $T itemcolumn $parent STATUS [list text [ mc "update" ] ]
        }
      }
    }
    lappend nodesInfo($nodeStatus) $nid
    foreach {{} d} $pkg(_.autodoc.dependencies) {
      set _nid [ $T insert [ list [ list back_to_ou_16 $d ] ] \
                    -at end -relative $nid -button no ]
      set nodesInfo($_nid,type) pkgdep
    }
  }
}

proc ::TolPkgGUI::CreateTreeWidget { t } {
  variable tree $t

  wtree $t -background white
  grid $t -row 1 -column 0 -sticky "snew"
  set f [ winfo parent $t ]
  grid rowconfigure $f 1 -weight 1
  grid columnconfigure $f 0 -weight 1
}

proc ::TolPkgGUI::CreateTree { t } {
  variable widgets

  CreateTreeWidget $t

  menu $t.cmenu -tearoff 0 \
      -postcommand [ list ::TolPkgGUI::PostContextMenu $t $t.cmenu ]
  $t configure -contextmenu $t.cmenu
}

proc ::TolPkgGUI::EntryState { listName } {
  upvar $listName v

  if { [ info exists v ] && [ llength $v ] } {
    return "normal"
  } else {
    return "disabled"
  }
}

proc ::TolPkgGUI::PostContextMenu { T w } {
  variable nodesInfo
  variable packSyncInfo
  variable versSyncInfo
  variable localVersionInfo
  variable localMostRecent

  $w delete 0 end
  array set selectionInfo {
    repo   ""
    pkg    ""
    pkgver ""
  }
  foreach nid [ $T selection get ] {
    lappend selectionInfo($nodesInfo($nid,type)) $nid
  }
  $w add command  -label [ mc "Add New Repository" ]... \
      -command ::TolPkgGUI::AddNewRepository
  set lenRepo [ llength $selectionInfo(repo) ]
  if { $lenRepo } {
    $w add cascade  -label [ mc "Check Repository" ] \
        -menu $w.check
    if { [ winfo exists $w.check ] } {
      $w.check delete 0 end
    } else {
      menu $w.check -tearoff 0
    }
    if { $lenRepo > 1 } {
      set labelCheck [ mc "Selected" ]
    } else {
      set labelCheck [ $T item text [ lindex $selectionInfo(repo) 0 ] 0 ]
    }
    $w.check add command -label $labelCheck \
        -command "::TolPkgGUI::CheckRepository $labelCheck $lenRepo"
    $w.check add command -label [ mc "All" ] \
        -command "::TolPkgGUI::CheckRepository ALL -1"
  }
  $w add command -label [ mc "Sync with servers" ] \
      -command "::TolPkgGUI::SyncServers"
  # preprocess selection of packages
  # list of new packages available
  set listNEW {}
  # list of package to remove including all installed versions
  set listDEL {}
  # list of individual package selected to remove
  set listDEL0 {}
  # list of packages selected needing update
  set listUPD {}
  # list of packages selected needing upgrade
  set listUPG {}
  # list of packages selected that can be exported  
  set listEXP {}
  
  # process first the list of pkg
  foreach nid $selectionInfo(pkg) {
    set pkg [ $T item text $nid 0 ]
    if { $nodesInfo($nid,status) ne "new" } {
      lappend listDEL [ list $pkg $nid ]
    }
    array unset pkgInfo
    if { [ info exists packSyncInfo($pkg) ] } {
      array set pkgInfo $packSyncInfo($pkg)
    } else {
      set pkgInfo(lastremote) $localMostRecent($pkg)
    }
    switch -exact $nodesInfo($nid,status) {
      "new" {
        lappend listNEW [ list $pkgInfo(lastremote) $nid ]
      }
      "upgrade" -
      "update" -
      "upgupd" {
        if { $nodesInfo($nid,status) eq "upgrade" ||
             $nodesInfo($nid,status) eq "upgupd" } {
          lappend listUPG [ list $pkg $nid ]
        }
        if { $nodesInfo($nid,status) eq "update" ||
             $nodesInfo($nid,status) eq "upgupd" } {
          foreach chId [ $T item children $nid ] {
            if { $nodesInfo($chId,status) eq "update" } {
              lappend listUPD [ list [ $T item text $chId 0 ] $chId ]
            }
          }
        }
      } 
      "ok" {
      }
      "unknown" {
      }
      default {
        puts "unexpected status $nodesInfo($nid,status) for pkg $pkg"
      }
    }
    foreach chId [ $T item children $nid ] {
      lappend listDEL0 [ list [ $T item text $chId 0 ] $chId ]
    }
    lappend listEXP [ list $pkgInfo(lastremote) $nodesInfo($nid,status) $nid ]
  }
  # next process the list of pkgver
  foreach nid $selectionInfo(pkgver) {
    set pkg [ $T item text $nid 0 ]
    switch -exact $nodesInfo($nid,status) {
      "new" {
        puts "OJO: he encontrado un estado new en un pkgver $pkg"
        if { [ lsearchi 0 $listNEW $pkg ] == -1 } {
          lappend listNEW [ list $pkg $nid ]
        }
      }
      "update" {
        if { [ lsearchi 0 $listUPD $pkg ] == -1 } {
          lappend listUPD [ list $pkg $nid ]
        }
      }
      "ok" {
      }
      default {
        puts "unexpected status $nodesInfo($nid,status) for pkg $pkg"
      }
    }
    if { [ lsearchi 0 $listDEL0 $pkg ] == -1 } {
      lappend listDEL0 [ list $pkg $nid ]
    }
    if { [ lsearchi 0 $listEXP $pkg ] == -1 } {
      lappend listEXP [ list $pkg $nodesInfo($nid,status) $nid ]
    }
  }
  # install list
  $w add separator
  if { [ llength $listNEW ] == 1 } {
    $w add command -label [ mc "Install %s" [ lindex [ lindex $listNEW 0 ] 0 ] ] \
        -command [ list ::TolPkgGUI::InstallPackages $listNEW ]
  } elseif { [ llength $listNEW ] > 1 } {
    $w add command -label [ mc "Install selected" ]... \
        -command [ list ::TolPkgGUI::InstallPackages $listNEW ]
  }
  $w add command -label [ mc "Install ZIP" ]... \
      -command "::TolPkgGUI::InstallZip"
  # update list
  if { [ llength $listUPD ] == 1 } {
    $w add command \
        -label [ mc "Update %s" [ lindex [ lindex $listUPD 0 ] 0 ] ] \
        -command [ list ::TolPkgGUI::UpdatePackageVersion $listUPD ]
  } elseif { [ llength $listUPD ] > 1 } {
    $w add command -label [ mc "Update selected" ]... \
        -command [ list ::TolPkgGUI::UpdatePackageVersion $listUPD ]
  }
  $w add command -label [ mc "Update all" ]... \
      -command "::TolPkgGUI::UpdatePackageVersion" \
      -state [ EntryState nodesInfo(update) ]
  # upgrade list
  $w add separator
  if { [ llength $listUPG ] == 1 } {
    $w add command \
        -label [ mc "Upgrade %s" [ lindex [ lindex $listUPG 0 ] 0 ] ] \
        -command [ list ::TolPkgGUI::UpgradePackages $listUPG ]
  } elseif { [ llength $listUPG ] > 1 } {
    $w add command -label [ mc "Upgrade selected" ]... \
        -command [ list ::TolPkgGUI::UpgradePackages $listUPG ]
  }
  $w add command -label [ mc "Upgrade all" ]... \
      -command "::TolPkgGUI::UpgradePackages" \
      -state [ EntryState nodesInfo(upgrade) ]

  # export options
  $w add separator
  if { [ llength $listEXP ] == 1 } {
    $w add command -label [ mc "Export %s" [ lindex [ lindex $listEXP 0 ] 0 ] ]... \
        -command [ list ::TolPkgGUI::ExportPackageVersion $listEXP ]
  } elseif { [ llength $listEXP ] > 1 } {
    $w add command -label [ mc "Export selected" ]... \
        -command [ list ::TolPkgGUI::ExportPackageVersion $listEXP ]
  }  
  $w add command -label [ mc "Export all" ]... \
      -command [ list ::TolPkgGUI::ExportPackageVersion ]
  # Remove options
  $w add separator
  if { [ llength $listDEL ] == 1 } {
    set _tmppkg [ lindex [ lindex $listDEL 0 ] 0 ]
    $w add command \
        -label [ mc "Remove all %s" $_tmppkg ]... \
        -command [ list ::TolPkgGUI::RemovePackageVersion $listDEL0 $_tmppkg ]
  } elseif { [ llength $listDEL0 ] == 1 } {
    set _tmppkg [ lindex [ lindex $listDEL0 0 ] 0 ]
    $w add command \
        -label [ mc "Remove %s" $_tmppkg ]... \
        -command [ list ::TolPkgGUI::RemovePackageVersion $listDEL0 ]
  } elseif { [ llength $listDEL0 ] > 1 } {
    $w add command -label [ mc "Remove selected" ]... \
        -command [ list ::TolPkgGUI::RemovePackageVersion $listDEL0 ]
  }  
  $w add command -label [ mc "Remove all" ]... \
      -command [ list ::TolPkgGUI::RemovePackageVersion ]
  
  # SyncInfo import/export
  $w add separator
  $w add command -label [ mc "Import SyncInfo" ]... \
      -command [ list ::TolPkgGUI::ImportSyncInfo ]
  $w add command -label [ mc "Export SyncInfo" ]... \
      -command [ list ::TolPkgGUI::ExportSyncInfo ]
  
}

proc ::TolPkgGUI::AddNewRepository { } {
  variable win
  variable urlmap

  set w $win.dlgAdd
  if { ![ winfo exist $w ] } {
    Dialog $w -title [ mc "Add repository"] \
        -parent $win -modal local
    set f [ $w getframe ]
    label $f.lb -text [ mc "Repository" ]:
    ComboBox $f.cb -values BPTP
    grid $f.lb -row 0 -column 0 -sticky ew
    grid $f.cb -row 0 -column 1 -sticky ew
    grid columnconfigure $f 1 -weight 1
    $w add -text Ok
    $w add -text Cancel
  }
  set ans [ $w draw ]
  if { $ans == 0 } {
    set repo [ string trim [ [$w getframe].cb get ] ]
    if { [ string range $repo 0 3 ] eq "http" } {
      set url $repo
    } else {
      if { [ info exists urlmap($repo) ] } {
        set url $urlmap($repo)
      } else {
        
      }
    }
    puts "::TolPkg::AddRepository $url"
    ::TolPkg::AddRepository $url
  }
}

proc ::TolPkgGUI::RefreshTree { } {
  variable tree

  $tree item delete all
  FillTreeInfo $tree
}

proc ::TolPkgGUI::SyncServers { } {
  variable tree
  update
  ::TolPkg::UpdateRepositoryInfo
  RefreshTree
}

proc ::TolPkgGUI::StartProgressBar { } {
  variable win
  variable Progress

  set Progress(status) "working"
  set Progress(state) 0
  $win configure -cursor watch
  foreach wc [ winfo children $win.dlgInstall ] {
    catch { $wc configure -cursor watch }
  }
}

proc ::TolPkgGUI::DoneProgress { } {
  variable win
  variable Progress 

  set Progress(status) "stopped"
  Dialog::enddialog $win.dlgInstall 1
  $win configure -cursor ""
  foreach wc [ winfo children $win.dlgInstall ] {
    catch { $wc configure -cursor "" }
  }
}

proc ::TolPkgGUI::AdvanceProgress { x } {
  variable Progress

  if { $Progress(status) eq "working" } {
    set Progress(state) $x
    update
  }
}

proc ::TolPkgGUI::InstallBtn1 { remote } {
  variable win
  variable Progress

  set w $win.dlgInstall
  $w itemconfigure 0 -state disabled
  $w itemconfigure 1 -command ::TolPkgGUI::DoneProgress
  set f [ $w getframe ]
  $f.lb1 configure -text [ mc "Installing" ]...
  InstallTask $remote
}

proc ::TolPkgGUI::DlgProcessBtn1 { } {
  variable win
  variable DlgProcessData

  set w [ DlgProcessGetWin ]
  # disable button Ok
  $w itemconfigure 0 -state disabled
  # set cancel to finish progress
  $w itemconfigure 1 -command ::TolPkgGUI::DlgProcessStop
  eval $DlgProcessData(-beforestart)
  DlgProcessTask 
}

proc ::TolPkgGUI::DlgProcess { pkgs args } {
  variable win
  variable DlgProcessData

  array unset DlgProcessData
  array set DlgProcessData {
    -title "Process packages"
    -beforestart ""
    -cmditem ""
    -cmdlabelitem ""
    -customgui ""
    -refreshtree 0
    -syncservers 0
  }
  set DlgProcessData(-label) [ mc "You are about to process"]:
  array set DlgProcessData $args
  set DlgProcessData(items) $pkgs
  set DlgProcessData(status) "stopped"
  set DlgProcessData(state) ""
  set w [ DlgProcessGetWin ]
  if { ![ winfo exists $w ] } {
    Dialog $w -title $DlgProcessData(-title) \
        -parent $win -modal local -cancel 1 -separator 1
    set f [ $w getframe ]
    label $f.lb1
    listbox $f.list -selectmode single -background white \
        -relief flat -borderwidth 1 -highlightthickness 0 \
        -selectbackground #c3c3ff
    frame $f.cgui
    ProgressBar $f.pbar
    $w add -text Ok 
    $w add -text Cancel
    grid $f.lb1 -row 0 -column 0 -sticky w
    grid $f.list -row 1 -column 0 -sticky snew
    grid $f.pbar -row 2 -column 0 -sticky e
    grid columnconfigure $f 0 -weight 1
    grid rowconfigure $f 1 -weight 1
  } else {
    wm title $w $DlgProcessData(-title)
    set f [ DlgProcessGetFrame ]
  }
  if { $DlgProcessData(-customgui) ne "" } {
    grid $f.cgui -row 3 -column 0 -sticky snew
    eval $DlgProcessData(-customgui) $f.cgui
  } else {
    grid remove $f.cgui
    grid rowconfigure $f 3 -weight 0
  }
  $f.pbar configure -maximum 1 \
      -variable ::TolPkgGUI::DlgProcessData(state,0)
  $f.pbar configure -variable ::TolPkgGUI::DlgProcessData(state)
  
  $f.lb1 configure -text $DlgProcessData(-label)
  $w itemconfigure 0 -state normal \
      -command [ list ::TolPkgGUI::DlgProcessBtn1 ]
  $w itemconfigure 1 -command "Dialog::enddialog $w 1"
  set lb [ DlgProcessGetList ]
  $lb delete 0 end
  foreach it $DlgProcessData(items) {
    if { $DlgProcessData(-cmdlabelitem) ne "" } {
      set labelIt [ $DlgProcessData(-cmdlabelitems) $it ]
    } else {
      set labelIt $it
    }
    $f.list insert end $it
  }
  set DlgProcessData(state) ""
  $w draw
  foreach wc [ winfo children $f.cgui ] {
    destroy $wc
  }
}

proc ::TolPkgGUI::DlgProcessGetCurrentIndex { } {
  variable DlgProcessData

  return $DlgProcessData(current)
}

proc ::TolPkgGUI::DlgProcessStart { } {
  variable win
  variable DlgProcessData

  set lb [ DlgProcessGetList ]
  set total [ llength $DlgProcessData(items) ]
  set DlgProcessData(current) 0
  $lb selection clear 0 end
  $lb selection set 0
  $lb see 0
  set DlgProcessData(status) "working"
  set DlgProcessData(state) 0
  $win configure -cursor watch
  set w [ DlgProcessGetWin ]
  foreach wc [ winfo children $w ] {
    catch { $wc configure -cursor watch }
  }
}

proc ::TolPkgGUI::DlgProcessAdvance { } {
  variable DlgProcessData

  set i [ incr DlgProcessData(current) ]
  set lb [ DlgProcessGetList ]
  set total [ llength $DlgProcessData(items) ]
  $lb selection set $i
  $lb selection anchor $i
  $lb see $i
  if { $DlgProcessData(status) eq "working" } {
    set DlgProcessData(state) [ expr { double($i) / $total } ]
    update
  }
}

proc ::TolPkgGUI::DlgProcessItem { p } {
  variable DlgProcessData

  if { $DlgProcessData(-cmditem) ne "" } {
    $DlgProcessData(-cmditem) $p
  }
}

proc ::TolPkgGUI::DlgProcessStop { } {
  variable win
  variable DlgProcessData
  
  set w [ DlgProcessGetWin ]
  $win configure -cursor ""
  foreach wc [ winfo children $w ] {
    catch { $wc configure -cursor "" }
  }
  set DlgProcessData(status) "stopped"
  Dialog::enddialog $w 1
}

proc ::TolPkgGUI::DlgProcessGetWin { } {
  variable win

  return $win.dlgProcess
}

proc ::TolPkgGUI::DlgProcessGetFrame { } {
  variable win

  set w [ DlgProcessGetWin ]
  return [ $w getframe ]
}

proc ::TolPkgGUI::DlgProcessGetList { } {
  variable win

  set w [ DlgProcessGetFrame ]
  return $w.list
}

proc ::TolPkgGUI::DlgProcessSetLabel { text } {
  variable win
  
  set f [ DlgProcessGetFrame ]
  $f.lb1 configure -text $text
}

proc ::TolPkgGUI::InstallPackages { pkgs } {
  variable win
  variable installData

  array unset installData
  foreach p $pkgs {
    array set installData $p
    lappend _pkgs [ lindex $p 0 ]
  }
  set InstallList [ TolPkg::CheckPackagesToInstall $_pkgs ]
  if { ![ llength $InstallList ] } {
    set ans [ MessageDlg $win.ask -title [ mc "Install packages" ] \
                  -icon error \
                  -parent $win \
                  -type ok -aspect 75 \
                  -message [ mc "Could not connect with remote repository" ] ]
    return
  }
  puts "InstallPackages: $InstallList"
  DlgProcess $InstallList -title [ mc "Install packages" ] \
      -label [ mc "You are about to install"]: \
      -beforestart ::TolPkgGUI::Installing \
      -cmditem "InstallThisPackage" \
      -syncservers 1
}

proc ::TolPkgGUI::DlgProcessGetVariableName { name } {
  return ::TolPkgGUI::DlgProcessData($name)
}

proc ::TolPkgGUI::DlgProcessGetVariable { name } {
  variable DlgProcessData

  return $DlgProcessData($name)
}

proc ::TolPkgGUI::DlgProcessTask { } {
  variable win
  variable Progress
  variable DlgProcessData
  
  DlgProcessStart
  set hasProcessed 0
  foreach p $DlgProcessData(items) {
    if { $DlgProcessData(status) ne "working" } break
    DlgProcessItem $p
    DlgProcessAdvance
    set hasProcessed 1
  }
  DlgProcessStop
  if { $hasProcessed } {
    if { $DlgProcessData(-syncservers) } {
      ::TolPkg::UpdateRepositoryInfo
      set DlgProcessData(-refreshtree) 1
    }
    if { $DlgProcessData(-refreshtree) } {
      after idle ::TolPkgGUI::RefreshTree
    }
  }
}

proc ::TolPkgGUI::InstallTask { remote } {
  variable win
  variable Progress
  variable InstallList
  
  set w $win.dlgInstall
  set f [ $w getframe ]
  set total [ llength $InstallList ]
  set i 0
  $f.list selection clear 0 end
  $f.list selection set 0
  $f.list see 0
  StartProgressBar
  foreach p $InstallList {
    if { $Progress(status) ne "working" } break
    if { $remote } {
      TolPkg::RemoteInstall $p
    } else {
      TolPkg::ZipInstall $p
    }
    incr i
    $f.list selection set $i
    $f.list selection anchor $i
    $f.list see $i
    AdvanceProgress [ expr { double($i) / $total } ]
  }
  DoneProgress
  
}

proc ::TolPkgGUI::InstallZip { } {
  variable win
  variable installZipData

  set installZipData [ tk_getOpenFile -defaultextension .zip -multiple 1 \
                           -parent $win -title [ mc "Select zip packages" ] \
                           -filetypes {{zip {.zip}}} ]
  if { $installZipData ne "" } {
    foreach f $installZipData {
      lappend zipList [ file tail $f ]
    }
    DlgProcess $zipList -title [ mc "Install Zip's" ] \
        -label [ mc "You are about to install zip"]: \
        -beforestart ::TolPkgGUI::InstallingZip \
        -cmditem InstallThisZip -syncservers 1
  }
}

proc ::TolPkgGUI::CheckConnectionConfig { } {
  variable win

  array set connConfig [ ::TolPkg::GetUpgradeConfig ]
  if { $connConfig(localPKG) } {
    MessageDlg $win.ask -title [ mc "Warning" ] \
        -icon warning \
        -type ok \
        -message [ mc "Can not continue. You have configured to use only local packages. Check the value of\nTolConfigManager::Config::Upgrading::TolPackage::LocalOnly." ]
    return 0
  }
  return 1
}

proc ::TolPkgGUI::UpgradePackages { { pkgs {} } } {
  variable installData
  variable nodesInfo
  variable tree

  if { ![ CheckConnectionConfig ] } {
    return 
  }
  array unset installData
  if { [ llength $pkgs ] } {
    foreach p $pkgs {
      array set installData $p
      lappend upgradeList [ lindex $p 0 ]
    }
  } else {
    # upgrade all, look for the list
    if { [ info exists nodesInfo(upgrade) ] } {
      foreach nid $nodesInfo(upgrade) {
        set p [ $tree item text $nid 0 ]
        lappend upgradeList $p
        set installData($p) $nid
      }
    } else {
      return
    }
  }
  DlgProcess $upgradeList -title [ mc "Upgrade packages" ] \
      -label [ mc "You are about to upgrade"]: \
      -beforestart ::TolPkgGUI::Upgrading \
      -cmditem "InstallThisPackage" -syncservers 1
}

proc ::TolPkgGUI::UpdatePackageVersion { { pkgs {} } } {
  variable installData
  variable nodesInfo
  variable tree

  puts "UpdatePackageVersion: $pkgs"
  if { ![ CheckConnectionConfig ] } {
    return 
  }
  
  array unset installData
  set updateList {}
  if { [ llength $pkgs ] } {
    foreach p $pkgs {
      array set installData $p
      lappend updateList [ lindex $p 0 ]
    }
  } else {
    # update all, look for the list
    if { [ info exists nodesInfo(update) ] } {
      foreach nid $nodesInfo(update) {
        set p [ $tree item text $nid 0 ]
        lappend updateList $p
        set installData($p) $nid
      }
    } else {
      return
    }
  }
  DlgProcess $updateList -title [ mc "Update packages" ] \
      -label [ mc "You are about to update"]: \
      -beforestart ::TolPkgGUI::Updating \
      -cmditem "InstallThisPackage" -syncservers 1
}

proc ::TolPkgGUI::RemovePackageVersion { { pkgs {} } { matchPkg {} } } {
  variable installData
  variable tree

  set removeList {}
  if { [ llength $pkgs ] } {
    foreach p $pkgs {
      set _p [ lindex $p 0 ]
      if { $matchPkg eq "" || [ regexp "^${matchPkg}\\.(\\d+)\\.(\\d+)\$" \
                                    $_p ] } {
        lappend removeList $_p
      }
    }
  } elseif { $matchPkg ne "" } {
    return
  } else {
    foreach nid [ $tree item id {depth 2} ] {
      lappend removeList [ $tree item text $nid 0 ]
    }
  }
  if { ![ llength $removeList ] } {
    return
  }
  array unset installData
  DlgProcess $removeList -title [ mc "Remove packages" ] \
      -label [ mc "You are about to remove"]: \
      -beforestart ::TolPkgGUI::Removing \
      -cmditem "RemoveThisPackage" -syncservers 1
}

proc ::TolPkgGUI::ImportSyncInfo { } {
  Unimplemented "Import Sync Info"
}

proc ::TolPkgGUI::ExportSyncInfo { } {
  Unimplemented "Export Sync Info"
}

proc ::TolPkgGUI::Unimplemented { title } {
  variable win

  MessageDlg $win.ask -parent $win -title [ mc $title ] \
      -icon warning \
      -type ok -message [ mc "Unimplemented option" ]
}

##
# Export packages --
#
# p == "" then the last installed version of every package is
# exported,
#
# [ llength $p ]>0 then all specified package is exported, if some
# packages is not installed then it is requested from the remote
# repository.
#

# Funciones que hacen falta:
#
#  1- Crear un zip de un paquete local
#  2- Descargar un zip de un paquete remoto
#
proc ::TolPkgGUI::ExportPackageVersion { {pkgs ""} } {
  variable win

  if { ![ CheckConnectionConfig ] } {
    return 
  }
  if { ![ llength $pkgs ] } {
    Unimplemented "Export all packages"
    return
  }
  set remote {}
  set local {}
  foreach _p $pkgs {
    foreach { p s } $_p break
    if {  $s eq "upgupd" || $s eq "upgrade" || $s eq "new" } {
      lappend remote $p
    } else {
      lappend local $p
    }
  }
  if { [ llength $remote ] } {
    set ans [ MessageDlg $win.ask -title [ mc "Export packages" ] \
                  -icon warning \
                  -type okcancel -aspect 75 \
                  -message [ mc "The following package can not be exported because they are not installed:\n%s\n\nContinue?" $remote ] ]
    if { $ans == 1 } {
      return
    }
  }
  puts "ExportPackageVersion: $local"
  if { [ llength $local ] } {
    DlgProcess $local -title [ mc "Export packages" ] \
        -label [ mc "You are about to export"]: \
        -beforestart ::TolPkgGUI::Exporting \
        -customgui "CreateSelectDir" \
        -cmditem "ExportThisPackage"
  }
}

proc ::TolPkgGUI::Exporting { } {
  DlgProcessSetLabel [ mc "Exporting" ]...
}

proc ::TolPkgGUI::Installing { } {
  DlgProcessSetLabel [ mc "Installing" ]...
}

proc ::TolPkgGUI::Upgrading { } {
  DlgProcessSetLabel [ mc "Upgrading" ]...
}

proc ::TolPkgGUI::Removing { } {
  DlgProcessSetLabel [ mc "Removing" ]...
}

proc ::TolPkgGUI::Updating { } {
  DlgProcessSetLabel [ mc "Updating" ]...
}

proc ::TolPkgGUI::InstallingZip { } {
  DlgProcessSetLabel [ mc "Installing Zip" ]...
}

proc ::TolPkgGUI::CreateSelectDir { f } {
  set varname [ DlgProcessGetVariableName "directory" ]
  upvar \#0 $varname var


  if { ![ info exists $varname ] } {
    trace add variable $varname write ::TolPkgGUI::OnChangeDirectory
  }
  label $f.lb -text [ mc "Output directory" ]:
  entry $f.ent -textvariable $varname
  button $f.btn -image folder_yellow_16 \
      -command "::TolPkgGUI::ChooseDir $varname"
  grid $f.lb -row 0 -column 0 -sticky w
  grid $f.ent -row 0 -column 1 -sticky ew
  grid $f.btn -row 0 -column 2 -sticky ew
  grid columnconfigure $f 1 -weight 1
  if { $var eq "" } {
    set var [ pwd ]
  } else {
    set var $var
  }
}

proc ::TolPkgGUI::OnChangeDirectory { args } {
  set w [ DlgProcessGetWin ]
  set dir [ DlgProcessGetVariable directory ]
  if { [ file exists $dir ] && [ file writable $dir ] } {
    $w itemconfigure 0 -state normal
  } else {
    $w itemconfigure 0 -state disabled
  }
}

proc ::TolPkgGUI::ChooseDir { varname } {
  variable win
  
  puts "upvar \#0 $varname var"
  upvar \#0 $varname var

  set res [ tk_chooseDirectory -parent $win -initialdir $var \
                -mustexist 1 -title [ mc "Select output directory" ] ]
  if { $res ne "" } {
    set var $res
  }
}

proc ::TolPkgGUI::ExportThisPackage { p } {
  set status [ TolPkg::ExportPackage $p [ DlgProcessGetVariable "directory" ] ]
  puts "::TolPkgGUI::ExportThisPackage $p ==> $status"
}

proc ::TolPkgGUI::InstallThisPackage { p } {
  variable win
  variable urlmap
  variable installData
  
  if { [ info exists installData($p) ] } {
    set nid $installData($p)
    set REPO [ $win.tree item text $nid REPO ]
    set repo $urlmap($REPO)
  } else {
    # packages requested as a dependencie
    set repo ""
  }
  set result [ TolPkg::RemoteInstall $p $repo ]
  puts "TolPkg::RemoteInstall $p: $result"
  return $result
}

proc ::TolPkgGUI::RemoveThisPackage { p } {
  set result [ TolPkg::RemovePackage $p ]
  puts "TolPkg::RemovePackage $p: $result"
  return $result
}

proc ::TolPkgGUI::InstallThisZip { p } {
  variable win
  variable urlmap
  variable installZipData
  
  puts "::TolPkgGUI::InstallThisZip $p"
  set idx [ DlgProcessGetCurrentIndex ]
  set thisZip [ lindex $installZipData $idx ]
  set result [ TolPkg::ZipInstall $thisZip ]
  puts "::TolPkgGUI::InstallThisZip $p: $result"
}

proc ::TolPkgGUI::Show { } {
  variable win

  set win .tolpkg

  if { [ winfo exists $win ] } {
    raise $win
  } else {
    toplevel $win
    wm state $win withdrawn
    wm title $win [mc "TOL Packages"]
    CreateTree $win.tree
    ::TolPkg::UpdateRepositoryInfo
    FillTreeInfo $win.tree
    wm state $win normal
  }
}

proc ::TolPkgGUI::test { } {
  destroy .tolpkg
  ::TolPkgGUI::Show
}

if { 0 } {
  ::TolPkg::UpdateRepositoryInfo
  ::TolPkgGUI::test
}