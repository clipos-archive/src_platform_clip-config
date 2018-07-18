#!/usr/bin/perl
# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright © 2009-2018 ANSSI. All Rights Reserved.
#######################################################################
# list_optional.pl - Produce a list of optional packages and their
# descriptions.
# Copyright (C) 2009 SGDN/DCSSI
# Copyright (C) 2011-2012 SGDSN/ANSSI
# Author: Vincent Strubel <clipos@ssi.gouv.fr>
#
# All rights reserved.
#
#######################################################################


use strict;
use warnings;
use Getopt::Long;
use CLIP::Pkg::Base ':all';
use CLIP::Conf::Base ':all';
use File::Path;

use CLIP::Logger ':all';
use Sys::Syslog qw(:macros);

###############################################################
#                    Global vars                              #
###############################################################


###### Consts ######
# Match possible jails with distribution
my %g_jails = (	
		clip	=>	"clip", 
		rm_h	=>	"rm", 
		rm_b	=>	"rm",
);

my $g_descname = "Description-fr";
my $g_catname = "Category-fr";
my $g_nocat = "Pas de catégorie";
my $g_jailsname = "CLIP-Jails";

###### Paths ######

$g_log_prefix = "list_optional";

$g_log_debug = 0;

$g_log_syslog = 0;
$CLIP::Logger::g_facilities->{"warn"} = LOG_LOCAL7;
$CLIP::Logger::g_facilities->{"log"} = LOG_LOCAL7;

# Defined based on command line
my $g_jail = "";
my $g_conf_file = ""; 
my $g_mirror_dir = "";
my $g_apt_config = "";
my $g_conf_pkg = "";
my $g_optional_pkg_file = "";
my $g_force = 0;
my $g_cache_file = "";

my %g_selected = ();
my %g_opthash = ();

my $g_cleanup = 0;

###### Command line ######

sub usage($) {
	my $fh = shift;

	print $fh "Usage: list_optional.pl -j <jail> [options]\n";
	print $fh "Options:\n";
	print $fh "   -cache <file>      : path to optional list cache\n";
	print $fh "   -conf <conf>       : path to apt.conf file\n";
	print $fh "   -j[ail] <jail>     : list packages for jail <jail>\n";
	print $fh "   -m[irror] <path>   : path to mirror base directory\n";
	print $fh "   -o[ptional] <file> : input optional file\n";
	print $fh "   -f[orce]           : force regeneration of the package list cache\n";
}

sub merror($) {
	my $msg = shift;

	clip_warn $msg;
	exit 1;
}

sub get_options() {
	eval {
		GetOptions (
			'help'		=>	sub {usage(*STDOUT{IO}); exit 0},
			'cache=s'	=>	\$g_cache_file,
			'conf=s'	=>	\$g_apt_config,
			'jail=s'	=>	\$g_jail,
			'mirror=s'	=>	\$g_mirror_dir,
			'optional=s'	=>	\$g_optional_pkg_file,
			'force'		=>	\$g_force,
			'Verbose'	=>	\$g_log_debug,
		) or die 1;
	};

	return 0 if ($@);

	merror "missing jail argument" if (not $g_jail);
	merror "missing cache path" if (not $g_cache_file);

	merror "invalid jail argument: $g_jail" 
			unless (grep {$_ eq $g_jail} (keys %g_jails));
	$g_apt_config="/usr/local/share/clip-config/apt.conf.$g_jail" 
				unless ($g_apt_config);

	if ($g_jail eq "clip") {
		$g_mirror_dir="/var/pkg"
			unless ($g_mirror_dir);
		$g_optional_pkg_file = 
			"/etc/admin/clip_install/optional.conf.clip"
				unless ($g_optional_pkg_file);
	} else {
		$g_mirror_dir="/var/pkg/$g_jail"
			unless ($g_mirror_dir);
		$g_optional_pkg_file = 
			 "/etc/admin/$g_jail/clip_install/optional.conf.rm"
				unless ($g_optional_pkg_file);
	}
	$g_conf_pkg = clippkg_get_confname($g_jails{$g_jail}, "apps", $g_jail);

	$CLIP::Pkg::Base::g_pkg_opts->{'Distribution'} = $g_jails{$g_jail};
	# NB: No Impact / Priority checks on download

	return 1;
}

sub check_option_files() {
	merror "missing apt config file $g_apt_config"
					if (not -f $g_apt_config);
}

sub get_config() {
	my $tmp; 

	## Apt / dpkg paths ##
	# Relaxed regexps, these are trusted files
	if (not defined ($tmp = clip_import_conf_sep($g_apt_config, "Dir::Cache", "\\S+", " "))) {
		merror "could not read apt cache path";
	} else {
		$tmp =~ s/["";]//g;
		$CLIP::Pkg::Download::g_apt_cache_dir = $tmp;
	}

	if (not defined ($tmp = clip_import_conf_sep($g_apt_config, "Dir::State::Status", "\\S+", " "))) {
		merror "could not read dpkg admin path";
	} else {
		$tmp =~ s/["";]//g;
		$tmp =~ s/\/status$//;
		$CLIP::Pkg::Base::g_dpkg_admin_dir = $tmp;
	}

	if ( -e $g_optional_pkg_file ) {
		open IN, "<", "$g_optional_pkg_file" or return 0;
		my @output = <IN>;
		close IN;
		foreach (@output) {
			chomp;
			$g_selected{$_} = 'y';
		}
	}
}

sub cleanup() {
	return 1 unless ($g_cleanup);

	my $cache = $CLIP::Pkg::Download::g_apt_cache_dir;
	my $admin = $CLIP::Pkg::Base::g_dpkg_admin_dir;

	rmtree($cache) if (-d $cache);
	rmtree($admin) if (-d $admin);
}

sub check_apt_tree() {
	my $cache = $CLIP::Pkg::Download::g_apt_cache_dir;
	my $admin = $CLIP::Pkg::Base::g_dpkg_admin_dir;

	# Fast path - apt tree already exists
	return 1 if (-d $cache and -d $admin);

	$g_cleanup = 1;

	# Slow path - create apt tree
	mkpath($cache."/archives/partial") 
		or merror "Could not create archives/partial";
	mkpath($cache."/lists/partial") 
		or merror "Could not create list/partial";
	mkpath($admin) 
		or merror "Could not create dpkg admin dir";

	open OUT, ">", $admin."/status" 
		or merror "Could not create dpkg status file";
	close OUT;

	return 1;
}

sub generate_cache_hash($) {
	my $outfile = shift;
	my $fields;

	clippkg_update_db(0) or merror "Could not update mirror";

	merror "Could not get fields for $g_conf_pkg"
		unless (defined($fields = clippkg_cache_get_fields($g_conf_pkg)));

	unless (defined ($fields->{"Suggests"})) {
		clip_warn "No optional packages for jail $g_jail";
		exit 0;
	}

	my $optionals = $fields->{"Suggests"};
	$optionals =~ s/\([^\)]+\)//g;
	$optionals =~ s/,//g;
	$optionals =~ s/  / /g;
	my @optlist = split " ", $optionals;

	PKGLOOP:
	foreach my $pname (@optlist) {
		my $pfields;
		unless (defined($pfields = clippkg_cache_get_all_fields($pname))) {
			clip_warn "No fields for $pname";
			next PKGLOOP;
		}

		if (defined($pfields->{$g_jailsname})) {
			my @jails = split ":", $pfields->{$g_jailsname};
			unless (grep {/^$g_jail$/} @jails) {
				clip_warn "$pname not for jail $g_jail";
				next PKGLOOP;
			}
		}

		my $cat;
		my $desc;

		if (defined($pfields->{$g_descname})) {
			$desc = $pfields->{$g_descname};
		} elsif (defined($pfields->{lc($g_descname)})) {
			# dpkg, etc.. tend to break the case in field names...
			$desc = $pfields->{lc($g_descname)};
		} else {
			next PKGLOOP;
		}

		if (defined($pfields->{$g_catname})) {
			$cat = $pfields->{$g_catname};
		} elsif (defined($pfields->{lc($g_catname)})) {
			# dpkg, etc.. tend to break the case in field names...
			$cat = $pfields->{lc($g_catname)};
		} else {
			$cat = $g_nocat;
		}

		if (defined($g_opthash{$cat})) {
			$g_opthash{$cat}->{$pname} = $desc;
		} else {
			$g_opthash{$cat} = { $pname => $desc };
		}
	}

	# Cache results
	open OUT, ">", $outfile or merror "Could not open $outfile for writing";

	my $hash;
	CATLOOP:
	foreach my $cat (sort keys %g_opthash) {
		next CATLOOP if ($cat eq $g_nocat);
		$hash = $g_opthash{$cat};
		foreach my $pname (sort keys %{$hash}) {
			print OUT "$cat//$pname $hash->{$pname}\n";
		}
	}
	if (defined($g_opthash{$g_nocat})) {
		$hash = $g_opthash{$g_nocat};
		foreach my $pname (sort keys %{$hash}) {
			print OUT "$g_nocat//$pname $hash->{$pname}\n";
		}
	}
	close OUT;
	cleanup();
}

sub read_cached_hash($) {
	my $infile = shift;

	unless (open IN, "<", $infile) {
		clip_warn "Cache file $infile not found, will have to generate it";
		return 0;
	}

	foreach my $line (<IN>) {
		if ($line =~ /^([^\/]+)\/\/(\S+) (.+)$/) {
			my $cat = $1;
			my $pname = $2;
			my $desc = $3;
			if (defined($g_opthash{$cat})) {
				$g_opthash{$cat}->{$pname} = $desc;
			} else {
				$g_opthash{$cat} = { $pname => $desc };
			}
		} else {
			clip_warn "Invalid line in cache $infile: $line";
			clip_warn "Cache will have to be regenerated";
			return 0;
		}
	}
	close IN;
	return 1;
}

###############################################################
#                    Main                                     #
###############################################################

clip_logger_init;
get_options or merror "failed to parse options";
check_option_files;
get_config;
$ENV{'APT_CONFIG'} = $g_apt_config;
check_apt_tree;

my $lock;
my $ret = 1;


if ($g_force) {
	generate_cache_hash($g_cache_file);
} else {
	generate_cache_hash($g_cache_file)
		unless (read_cached_hash($g_cache_file));
}

my $hash;
CATLOOP:
foreach my $cat (sort keys %g_opthash) {
	next CATLOOP if ($cat eq $g_nocat);
	print "*$cat\n";
	$hash = $g_opthash{$cat};
	foreach my $pname (sort keys %{$hash}) {
		if (defined ($g_selected{$pname})) {
			print "+$pname $hash->{$pname}\n";
		} else {
			print "-$pname $hash->{$pname}\n";
		}
	}
}

if (defined($g_opthash{$g_nocat})) {
	$hash = $g_opthash{$g_nocat};
	print "*$g_nocat\n";
	foreach my $pname (sort keys %{$hash}) {
		if (defined ($g_selected{$pname})) {
			print "+$pname $hash->{$pname}\n";
		} else {
			print "-$pname $hash->{$pname}\n";
		}
	}
}
