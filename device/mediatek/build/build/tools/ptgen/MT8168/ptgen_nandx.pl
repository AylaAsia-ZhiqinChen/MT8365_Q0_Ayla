#!/usr/local/bin/perl -w
#
#****************************************************************************/
#* This script will generate partition layout files
#* Author: Bean Li
#*
#*	History:
#*		[2018/07/27] Create first ptgen nandx version, refactor ptgen code
#		[2018/08/06] Modify scatter file config version to V1.1.7
#****************************************************************************/

BEGIN
{
	use strict;
	use File::Basename;
	use File::Path;
	use File::Copy;
	use FindBin qw($Bin);
	use lib "$Bin/../../Spreadsheet";
	require 'ParseExcel.pm';
}

#*
# Global Variable Define
#*
my $VERSION = "1.0";
my $LOCAL_PATH = dirname($0);
my $SHEET_NAME;
my %config_list;
my %file_list;
my %board_config_list;
my @partition_layout;
my @scatter_layout;
my @mntl_gpt_layout;

&ptgen_main();

END
{
	print "\nptgen nandx done. \n"
}

#******************************** Subroutine Define Start Here ********************************/

sub ptgen_main
{
	&runtime_check();
	&xls_data_read();
	&scatter_layout_create();
	&scatter_file_write();
	&copy_files();
	exit;
}

#*
#	Config Module Start
#*

sub runtime_check
{
	print "\nWelcome to ptgen nandx! \n";
	print "Version:$VERSION \n";
	print "Path:$LOCAL_PATH \n";

	&config_list_init();
	&file_list_init();

	if ($config_list{COMBO_NAND_SUPPORT} eq "yes" &&
		($config_list{MLC_NAND_SUPPORT} eq "yes" ||
		 $config_list{TLC_NAND_SUPPORT} eq "yes")) {
		 print	"\nptgen nandx check ok and start .... \n\n";
		 $SHEET_NAME = "nandx";
		 if ($config_list{MNTL_SUPPORT} eq "yes") {
			&mntl_gpt_init();
		 }
	} else {
		print "\nptgen nandx cannot support it, please check your project configuration! \n";
		&error_print("sorry sorry sorry \n", __FILE__, __LINE__);
	}
}

sub copy_files
{
	copy_file($file_list{SCATTER_FILE}, $file_list{TARGET_SCATTER_DIR});
	copy_file($file_list{MNTL_GPT_FILE}, $file_list{TARGET_MNTL_GPT_DIR});
}

sub config_list_init
{
	$config_list{PLATFORM}            = $ENV{PLATFORM};
	$config_list{BASE_PROJECT}        = $ENV{MTK_BASE_PROJECT};
	$config_list{TARGET_PROJECT}      = $ENV{MTK_TARGET_PROJECT};
	$config_list{MLC_NAND_SUPPORT}    = $ENV{MTK_MLC_NAND_SUPPORT};
	$config_list{TLC_NAND_SUPPORT}    = $ENV{MTK_TLC_NAND_SUPPORT};
	$config_list{COMBO_NAND_SUPPORT}  = $ENV{MTK_COMBO_NAND_SUPPORT};
	$config_list{TEE_SUPPORT}         = $ENV{MTK_IN_HOUSE_TEE_SUPPORT};
	$config_list{MNTL_SUPPORT}        = $ENV{MNTL_SUPPORT};
	$config_list{DTBO_FEATURE}        = $ENV{MTK_DTBO_FEATURE};
	$config_list{TARGET_BUILD}        = $ENV{TARGET_BUILD_VARIANT};

	&config_list_dump();
}

sub config_list_dump
{
	print "\nconfig list dump: \n";
	print "PLATFORM=$config_list{PLATFORM} \n";
	print "MTK_BASE_PROJECT=$config_list{BASE_PROJECT} \n";
	print "MTK_TARGET_PROJECT=$config_list{TARGET_PROJECT} \n";
	print "MTK_MLC_NAND_SUPPORT=$config_list{MLC_NAND_SUPPORT} \n";
	print "MTK_TLC_NAND_SUPPORT=$config_list{TLC_NAND_SUPPORT} \n";
	print "MTK_COMBO_NAND_SUPPORT=$config_list{COMBO_NAND_SUPPORT} \n";
	print "MTK_IN_HOUSE_TEE_SUPPORT=$config_list{TEE_SUPPORT} \n";
	print "MNTL_SUPPORT=$config_list{MNTL_SUPPORT} \n";
	print "TARGET_BUILD=$config_list{TARGET_BUILD} \n";
}

sub file_list_init
{
	$file_list{PARTITION_TABLE} = "$LOCAL_PATH/partition_table_$config_list{PLATFORM}.xls";

	if (not exists $ENV{PTGEN_MK_OUT}) {
		&error_print("No PTGEN_MK_OUT!!!! \n", __FILE__, __LINE__);
	}
	$file_list{SCATTER_FILE} = "$ENV{PTGEN_MK_OUT}/$config_list{PLATFORM}_Android_scatter.txt";
	$file_list{MNTL_GPT_FILE} = "$ENV{PTGEN_MK_OUT}/partition_nand.xml";
	$file_list{PARTITION_SIZE_FILE} = "$ENV{PTGEN_MK_OUT}/partition_size.mk";

	if (not exists $ENV{OUT_DIR}) {
		&error_print("No OUT_DIR!!!! \n", __FILE__, __LINE__);
	}
	$file_list{TARGET_SCATTER_DIR} = "$ENV{OUT_DIR}/target/product/$config_list{TARGET_PROJECT}";
	$file_list{TARGET_MNTL_GPT_DIR} = "$ENV{OUT_DIR}/target/product/$config_list{TARGET_PROJECT}";

	&file_list_dump();
}

sub file_list_dump
{
	print "\nfile list dump: \n";
	print "PARTITION_TABLE=$file_list{PARTITION_TABLE} \n";
	print "SCATTER_FILE=$file_list{SCATTER_FILE} \n";
	print "TARGET_SCATTER_DIR=$file_list{TARGET_SCATTER_DIR} \n";
	print "MNTL_GPT_FILE=$file_list{MNTL_GPT_FILE} \n";
	print "TARGET_MNTL_GPT_DIR=$file_list{TARGET_MNTL_GPT_DIR} \n";
	print "PARTITION_SIZE_FILE=$file_list{PARTITION_SIZE_FILE} \n";
}

#*
#	Config Module End
#*

#*
#	Excel Module Start
#*
sub xls_data_read
{
	my @name      = ();
	my @sub_name  = ();
	my $sheet = &xls_sheet_read($file_list{PARTITION_TABLE});
	my ($row_min, $row_max) = $sheet->row_range();    #$row_min=0
	my ($col_min, $col_max) = $sheet->col_range();    #$col_min=0

	$col_min += 1; # skip 1 column Index
	foreach my $col_idx ($col_min .. $col_max) {
		push(@col_name,     &xls_cell_read($sheet, $row_min,     $col_idx));
		push(@col_sub_name, &xls_cell_read($sheet, $row_min + 1, $col_idx));
	}

	$row_min += 2; # skip 2 row Title
	foreach my $col_idx ($col_min .. $col_max) {
		foreach my $row_idx ($row_min .. $row_max) {
			my $value = &xls_cell_read($sheet, $row_idx, $col_idx);
			my $col_index = $col_idx - $col_min;
			my $row_index = $row_idx - $row_min;
			if ($col_name[$col_index]) {
				$partition_layout[$row_index]{$col_name[$col_index]} = $value;
			} else {
				$col_index -= 1;
				my $pre_value = $partition_layout[$row_index]{$col_name[$col_index]};
				if (!$value) {
					$value = $pre_value;
				}
				if ($config_list{TARGET_BUILD} eq "eng") {
					$partition_layout[$row_index]{$col_name[$col_index]} = $pre_value;
				} else {
					$partition_layout[$row_index]{$col_name[$col_index]} = $value;
				}
			}
		}
	}
}

sub xls_sheet_read
{
	my $path = shift @_;
	my $xls_book = Spreadsheet::ParseExcel->new()->Parse($path);
	my $sheet = $xls_book->Worksheet($SHEET_NAME);

	if (!$sheet) {
		&error_print("ptgen open sheet=$SHEET_NAME fail in $path \n", __FILE__, __LINE__);
	}

	return $sheet;
}

sub xls_cell_read
{
	my ($sheet, $row, $col) = @_;
	my $cell = $sheet->get_cell($row, $col);
	if (defined $cell) {
		return $cell->unformatted();
	} else {
		&error_print("excel read fail,(row=$row,col=$col) undefine \n", __FILE__, __LINE__);
	}
}

#*
#	Excel Module End
#*

#*
#	Scatter Module Start
#*

sub scatter_layout_create
{
	my $start_addr_kb = 0;
	my $scatter_iter = 0;

	&board_config_load();
	for ($iter = 0; $iter < @partition_layout; $iter++) {
		my %part = %{$partition_layout[$iter]};
		my $file_name = $part{Download_File};
		my $type = "NORMAL_ROM";
		my $format_start_addr;

		my $part_size = board_config_check($part{Partition_Name});
		if ($part_size) {
			print "Board config $part{Partition_Name} size $part{Size_KB} to $part_size \n";
			$part{Size_KB} = $part_size;
		}

		if ($part{Partition_Name} =~ /PRELOADER/) {
			$file_name = "preloader_$config_list{BASE_PROJECT}.bin";
			$type = "SV5_BL_BIN";
		}

		if ($config_list{TEE_SUPPORT} ne "yes" && $part{Partition_Name} =~ /TEE/) {
			next;
		}

		# USRDATA must be the last mnt partition and partition size must be zero
		if ($part{Type} eq "Mntl data") {
			&mntl_layout_create($iter);
			if ($part{Partition_Name} ne "USRDATA") {
				next;
			}
			$file_name = "mntl.img";
			$type = "FTL20_IMG";
		}

		# BMTPOOL must the last partition and partition size must be zero
		if ($part{Partition_Name} =~ /BMTPOOL/) {
			$format_start_addr = sprintf("0xffff%04x", $part{Size_KB});
		} else {
			$format_start_addr = sprintf("0x%x", $start_addr_kb * 1024);
		}

		$scatter_layout[$scatter_iter] = {
							partition_name      => $part{Partition_Name},
							file_name           => $file_name,
							is_download         => $part{Download} eq "Y" ? "true" : "false",
							type                => $part{Download} eq "Y" ? $type : "NONE",
							linear_start_addr   => $format_start_addr,
							physical_start_addr => $format_start_addr,
							partition_size      => sprintf("0x%x", $part{Size_KB} * 1024),
							region              => $part{Region},
							storage             => "HW_STORAGE_NAND",
							boundary_check      => $part{Reserved} eq "Y" ? "false" : "true",
							is_reserved         => $part{Reserved} eq "Y" ? "true" : "false",
							operation_type      => $part{Operation_Type},
							d_type              => $part{Type} eq "Raw data" ? "LOW_PAGE" : "FULL_PAGE",
							slc_percentage      => 0,
							is_upgradable       => $part{OTA_Update} eq "Y" ? "true" : "false",
							reserve             => "0x00"
						};

		if ($part{Size_KB}) {
			$start_addr_kb += $part{Size_KB};
		}
		$scatter_iter += 1;
	}
}

sub scatter_file_write
{
	my $scatter_fd = &file_open($file_list{SCATTER_FILE});
	my $Head1 = <<"__TEMPLATE";
############################################################################################################
#
#  General Setting
#
############################################################################################################
__TEMPLATE

	my $Head2=<<"__TEMPLATE";
############################################################################################################
#
#  Layout Setting
#
############################################################################################################
__TEMPLATE

	my ${FirstDashes}="- ";
	my ${FirstSpaceSymbol}="  ";
	my ${SecondSpaceSymbol}="      ";
	my ${SecondDashes}="    - ";
	my ${colon}=": ";

	print $scatter_fd $Head1;
	print $scatter_fd "${FirstDashes}general${colon}MTK_PLATFORM_CFG\n";
	print $scatter_fd "${FirstSpaceSymbol}info${colon}\n";
	print $scatter_fd "${SecondDashes}config_version${colon}V1.1.7\n";
	print $scatter_fd "${SecondSpaceSymbol}platform${colon}$config_list{PLATFORM}\n";
	print $scatter_fd "${SecondSpaceSymbol}project${colon}$config_list{TARGET_PROJECT}\n";
	print $scatter_fd "${SecondSpaceSymbol}storage${colon}NAND\n";
	print $scatter_fd "${SecondSpaceSymbol}boot_channel${colon}NONE\n";
	print $scatter_fd ("${SecondSpaceSymbol}block_size${colon}0x20000\n");
	print $scatter_fd $Head2;

	for ($iter = 0; $iter < @scatter_layout; $iter++) {
		my %scatter = %{$scatter_layout[$iter]};
		print $scatter_fd "${FirstDashes}partition_index${colon}SYS$iter\n";
		print $scatter_fd "${FirstSpaceSymbol}partition_name${colon}$scatter{partition_name}\n";
		print $scatter_fd "${FirstSpaceSymbol}file_name${colon}$scatter{file_name}\n";
		print $scatter_fd "${FirstSpaceSymbol}is_download${colon}$scatter{is_download}\n";
		print $scatter_fd "${FirstSpaceSymbol}type${colon}$scatter{type}\n";
		print $scatter_fd "${FirstSpaceSymbol}linear_start_addr${colon}$scatter{linear_start_addr}\n";
		print $scatter_fd "${FirstSpaceSymbol}physical_start_addr${colon}$scatter{physical_start_addr}\n";
		print $scatter_fd "${FirstSpaceSymbol}partition_size${colon}$scatter{partition_size}\n";
		print $scatter_fd "${FirstSpaceSymbol}region${colon}$scatter{region}\n";
		print $scatter_fd "${FirstSpaceSymbol}storage${colon}$scatter{storage}\n";
		print $scatter_fd "${FirstSpaceSymbol}boundary_check${colon}$scatter{boundary_check}\n";
		print $scatter_fd "${FirstSpaceSymbol}is_reserved${colon}$scatter{is_reserved}\n";
		print $scatter_fd "${FirstSpaceSymbol}operation_type${colon}$scatter{operation_type}\n";
		print $scatter_fd "${FirstSpaceSymbol}d_type${colon}$scatter{d_type}\n";
		print $scatter_fd "${FirstSpaceSymbol}slc_percentage${colon}$scatter{slc_percentage}\n";
		print $scatter_fd "${FirstSpaceSymbol}is_upgradable${colon}$scatter{is_upgradable}\n";
		print $scatter_fd "${FirstSpaceSymbol}reserve${colon}$scatter{reserve}\n\n";
	}

	&file_close($scatter_fd);

	if ($config_list{MNTL_SUPPORT} eq "yes") {
		&mntl_gpt_write();
		&mntl_partition_size_write();
	}
}

#*
#	Scatter Module End
#*

#*
#	Mntl Module Start
#*

sub mntl_gpt_init
{
	$mntl_gpt_layout[0]{Partition_Num} = 0;
	$mntl_gpt_layout[0]{Start_Sector} = 4;
	$mntl_gpt_layout[0]{Sector_Size} = 4096;
	$mntl_gpt_layout[0]{Hash_Name} = {
						ANDROID   => "mntlblk_d1",
						VENDOR    => "mntlblk_d2",
						CACHE     => "cache",
						USRDATA   => "userdata"
					};
	$mntl_gpt_layout[0]{Type} = "{0FC63DAF-8483-4772-8E79-3D69D8477DE4}";
}

sub mntl_layout_create
{
	my $part_index = shift @_;
	my $data_part_fake_size = 2 * 1024 * 1024 * 1024; # 2GB
	if ($config_list{MNTL_SUPPORT} ne "yes") {
		&error_print("MNTL_SUPPORT is not equal yes, but Type is Mntl data \n", __FILE__, __LINE__);
	}

	my %part = %{$partition_layout[$part_index]};
	$mntl_gpt_layout[0]{Partition_Num} += 1;
	my $iter = $mntl_gpt_layout[0]{Partition_Num};

	$mntl_gpt_layout[$iter]{Partition_Name} = $mntl_gpt_layout[0]{Hash_Name}{$part{Partition_Name}};
	$mntl_gpt_layout[$iter]{Start_Sector} = $mntl_gpt_layout[0]{Start_Sector};

	if ($part{Partition_Name} eq "USRDATA") {
		$mntl_gpt_layout[0]{Start_Sector} += $data_part_fake_size / $mntl_gpt_layout[0]{Sector_Size};
	} else {
		$mntl_gpt_layout[0]{Start_Sector} += $part{Size_KB} * 1024 / $mntl_gpt_layout[0]{Sector_Size};
	}

	$mntl_gpt_layout[$iter]{End_Sector} = $mntl_gpt_layout[0]{Start_Sector} - 1;
	$mntl_gpt_layout[$iter]{File_Name} = $part{Download_File};
}

sub mntl_gpt_write
{
	my $total_size = $mntl_gpt_layout[0]{Start_Sector} + $mntl_gpt_layout[1]{Start_Sector} - 1;
	my $mntl_fd = &file_open($file_list{MNTL_GPT_FILE});

	print $mntl_fd "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	print $mntl_fd "<partition lba=\"$total_size\">\n";
	for ($iter = 1; $iter < @mntl_gpt_layout; $iter++) {
		my %gpt = %{$mntl_gpt_layout[$iter]};
		print $mntl_fd "\t<entry type=\"$mntl_gpt_layout[0]{Type}\" ";
		print $mntl_fd "start=\"$gpt{Start_Sector}\" end=\"$gpt{End_Sector}\" name=\"$gpt{Partition_Name}\" file_name=\"$gpt{File_Name}\"";
		print $mntl_fd " />\n";
	}
	print $mntl_fd "</partition>\n";

	&file_close($mntl_fd);
}

# This subroutine is not needed, but reserved for build_image.py
sub mntl_partition_size_write
{
	my %hash_name = (
				"mntlblk_d1" => "SYSTEM",
				"mntlblk_d2" => "VENDOR",
				"cache"      => "CACHE",
				"userdata"   => "USERDATA"
			);
	my $part_fd = &file_open($file_list{PARTITION_SIZE_FILE});
	print $part_fd "BOARD_FLASH_PAGE_SIZE:=16384\n";
	for ($iter = 1; $iter < @mntl_gpt_layout; $iter++) {
		my %gpt = %{$mntl_gpt_layout[$iter]};
		my $part_name = $hash_name{$gpt{Partition_Name}};
		my $part_size = ($gpt{End_Sector} - $gpt{Start_Sector} + 1) * $mntl_gpt_layout[0]{Sector_Size};
		print $part_fd "BOARD_$part_name"."IMAGE_PARTITION_SIZE:=$part_size\n";
		print $part_fd "BOARD_$part_name"."IMAGE_START_SECTOR:=$gpt{Start_Sector}\n";
		print $part_fd "BOARD_$part_name"."IMAGE_END_SECTOR:=$gpt{End_Sector}\n";
	}
	my $total_size = $mntl_gpt_layout[0]{Start_Sector} + $mntl_gpt_layout[1]{Start_Sector} - 1;
	print $part_fd "BOARD_TOTAL_SECTORS:=$total_size\n";
	&file_close($part_fd);
}

#*
#	Mntl Module End
#*

#*
#	Board Config Module Start
#*

sub board_config_load
{
	my $board_config_path;
	my $dir_level0 = "device";
	my $file_name = "$config_list{TARGET_PROJECT}/BoardConfig.mk";

	opendir(DIR, $dir_level0) or &error_print("opendir $dir_level0 fail \n", __FILE__, __LINE__);
	while (my $sub_dir = readdir(DIR)) {
		next unless (-d "$dir_level0/$sub_dir");
		next unless ($sub_dir !~ m/^\./); #ignore dir prefixed with .
		next unless ($sub_dir =~ /mediatek/);
		$board_config_path = "$dir_level0/$sub_dir/$file_name";
		if (-e $board_config_path) {
			print "Board config find at $board_config_path \n";
			last;
		}
	}
	closedir(DIR);

	if (-e $board_config_path) {
		open my $file_fd, "< $board_config_path" or &error_print("Can not open $board_config_path for read \n", __FILE__, __LINE__);
		while (defined(my $line = <$file_fd>)) {
			if ($line =~ /\A\s*BOARD_MTK_(.*)_SIZE_KB\s*:=\s*(\d+)/i ||
				$line =~ /\A\s*BOARD_MTK_(.*)_SIZE_KB\s*:=\s*(NA)/i) {
				$board_config_list{$1} = $2;
			}
		}
		close $file_fd;
	}
}

sub board_config_check
{
	my $part_name = shift @_;
	foreach my $key_name (keys %board_config_list) {
		if ("$key_name" eq "$part_name") {
			print "borad config partition $part_name size to $board_config_list{$key_name}\n";
			return $board_config_list{$key_name};
		}
	}
}

#*
#	Board Config Module End
#*

#*
#	Util Module Start
#*

sub copy_file
{
	my ($src_file, $dst_path) = @_;
	my $file_name = substr($src_file, rindex($src_file, "/"), length($src_file));
	if (-e "$dst_path/$file_name") {
		chmod(0777, "$dst_path/$file_name");
	} else {
		eval { mkpath($dst_path) };
		if ($@) {
			&error_print("Can not make dir $dst_path \n", __FILE__, __LINE__, $@);
		}
	}
	print "Copy $src_file To $dst_path \n";
	eval { copy($src_file, $dst_path) };
	if ($@) {
		&error_print("Can not copy $src_file to $dst_path \n", __FILE__, __LINE__, $@);
	}
}

sub file_open
{
	my $file_path = shift @_;

	if (-e $file_path) {
		chmod(0777, $file_path) or &error_print("chmod 0777 $file_path fail \n", __FILE__, __LINE__);
		if (!unlink $file_path) {
			&error_print("remove $file_path fail \n", __FILE__, __LINE__);
		}
	} else {
		my $dir_path = substr($file_path, 0, rindex($file_path, "/"));
		eval { mkpath($dir_path) };
		if ($@) {
			&error_print("Can not make dir $dir_path \n", __FILE__, __LINE__, $@);
		}
	}

	open my $file_fd, "> $file_path" or &error_print(" Can not open $file_path for read and write \n", __FILE__, __LINE__);

	return $file_fd;
}

sub file_close
{
	my $file_fd = shift @_;

	close $file_fd;
}

sub error_print
{
	my ($error_msg, $file, $line_no, $sys_msg) = @_;
	if (!$sys_msg) {
		$sys_msg = $!;
	}
	print "Fatal error: $error_msg <file: $file,line: $line_no> : $sys_msg";
	die;
}

#*
#	Util Module End
#*
