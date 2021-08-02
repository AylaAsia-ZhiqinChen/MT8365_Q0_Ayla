use strict;
use warnings;

#****************************************************************************
# subroutine:  ReadCSVFile
# return:
#****************************************************************************

sub ReadCSVFile_Common
{
    my $csvFilePath = shift @_;
    my @partition_layout_raw;
    open(my $fh, $csvFilePath);
    my @col_name = ();
    my @col_sub_name = ();
    my @title_row;
    my @ext_title_row;
    my $row;

    # read title row
    $row = <$fh>;
    $row =~ s/\r?\n//g;
    @title_row = split(",", $row);
    $row = <$fh>;
    $row =~ s/\r?\n//g;
    @ext_title_row = split(",", $row);
    for (my $i = @ext_title_row; $i < @title_row; $i++) {
        $ext_title_row[$i] = "";
    }
    for (my $i = 0; $i < @title_row; $i++) {
        #print "[$i], $title_row[$i], $ext_title_row[$i]\n";
        if ($title_row[$i] eq "") {
            push(@col_name, undef);
        } else {
            push(@col_name, $title_row[$i]);
        }
        if ($ext_title_row[$i] eq "") {
            push(@col_sub_name, undef);
        } else {
            push(@col_sub_name, $ext_title_row[$i]);
        }
    }

    # read partition table
    while (my $row = <$fh>) {
        $row =~ s/\r?\n//g;
        my @row_data = split(",", $row);
        my %entry_info;
        my $prev_val;
        my $col_idx = 0;
        foreach my $val (@row_data) {
            #print "$col_idx $col_name[$col_idx] $col_sub_name[$col_idx] $val\n";
            if ($col_name[$col_idx])
            {
                $entry_info{$col_name[$col_idx]} = $val;
            }
            else
            {
                my $prev_val = $entry_info{$col_name[$col_idx-1]};
                if (!$val)
                {
                    $val = $prev_val;
                }
                $entry_info{$col_name[$col_idx-1]} = {$col_sub_name[$col_idx-1] => $prev_val, $col_sub_name[$col_idx] => $val};
            }
            $col_idx = $col_idx + 1;
        }
        push(@partition_layout_raw, \%entry_info);
    }
    return \@partition_layout_raw;
}

1;
