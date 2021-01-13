#include "DDMI.h"

int main(int argc, char **argv) {
	int ret = 1, i, j;
	BYTE bCount;
	DDMInfo Info[MAX_DDM_COUNT];
	DDMConfig Cfgo[MAX_DDM_COUNT];

	if (argc != 1 || strlen(argv[1]) != 1 || argv[1][0] != 'v' && argv[1][0] != 'n')
		return 1;
		
	if (!EkDDMiSMBGetDDMInfos(bCount, Info)) ;
	else {
		for (Count = bCount, i = 0; i < Count && strcmp(Info[i].Descript, "Logo"); i++) ;
		if (i >= Count || !Info[i].Exist) ;
		else if (!EkDDMiSMBGetDDMConfigs(bCount, Cfg)) ;
		else {
			for (Count = bCount, j = 0; j < Count && Cfg[j].ID != Info[i].ID; j++) ;
			if (j < Count) {
				if (argc == 1)
					Cfg[j].Visible = 1 - Cfg[j].Visible;
				else
					Cfg[j].Visible = argv[1][0] == 'v' ? TRUE : FALSE;
				if (EkDDMiSMBGetDDMConfig(Cfg[j])) {
					printf("Set Logo page success!\n");
					ret = 0;
				}
			}
		}
	}
	if (ret)
		printf("Set Logo page fail!\n");
	return ret;
}