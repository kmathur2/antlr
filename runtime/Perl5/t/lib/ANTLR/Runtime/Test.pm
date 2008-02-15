package ANTLR::Runtime::Test;

use strict;
use warnings;

use base 'Test::Builder::Module';

my $CLASS = __PACKAGE__;

our @EXPORT = qw( g_test_output_is );

use Cwd;
use File::Spec;
use File::Temp qw( tempdir );

sub read_file {
    my ($filename) = @_;

    local $/;
    open my $in, '<', $filename or die "Can't open $filename: $!";
    my $content = <$in>;
    close $in or warn "Can't close $filename: $!";

    return $content;
}

sub write_file {
    my ($filename, $content) = @_;

    open my $out, '>', $filename or die "Can't open $filename: $!";
    print $out $content;
    close $out or warn "Can't close $filename: $!";

    return;
}

sub g_test_output_is {
    my ($args) = @_;
    my $grammar = $args->{grammar};
    my $test_program = $args->{test_program};
    my $expected = $args->{expected};
    my $name = $args->{name} || undef;
    my $tb = $CLASS->builder;

    my $tmpdir = tempdir( CLEANUP => 1 );

    my $grammar_name;
    if ($grammar =~ /grammar (\w+)/) {
        $grammar_name = $1;
    } else {
        return 0;
    }

    # write grammar file
    my $grammar_file = File::Spec->catfile($tmpdir, "$grammar_name.g");
    write_file($grammar_file, $grammar);

    # write test program file
    my $test_program_file = File::Spec->catfile($tmpdir, 'test.pl');
    write_file($test_program_file, $test_program);

    my $cwd = cwd;
    chdir $tmpdir;

    # compile grammar
    my $g_result = run_program([ File::Spec->catfile($cwd, 'antlr'), $grammar_file ]);

    # run test program
    my $test_result = run_program([ 'perl', "-Mblib=$cwd", 'test.pl']);

    chdir $cwd;

    my $actual = $test_result->{out};

    # compare with $expected
    return $tb->is_eq($actual, $expected, $name);
}

sub run_program {
    my ($command) = @_;

    open my $old_out, '>&STDOUT' or die "Can't capture stdout: $!";
    close STDOUT or die "Can't close stdout: $!";
    open STDOUT, '>', 'out.tmp' or die "Can't redirect stdout: $!";

    open my $old_err, '>&STDERR' or die "Can't capture stderr: $!";
    close STDERR or die "Can't close stderr: $!";
    open STDERR, '>', 'err.tmp' or die "Can't redirect stderr: $!";

    system @$command;
    my $exit_code = $?;

    # restore stderr
    my $err = read_file('err.tmp');
    close STDERR or die "Can't close stderr: $!";
    open STDERR, '>&', $old_err or die "Can't restore stderr: $!";

    # restore stdout
    my $out = read_file('out.tmp');
    close STDOUT or die "Can't close stdout: $!";
    open STDOUT, '>&', $old_out or die "Can't restore stdout: $!";

    my $exit_value;
    if ($exit_code < 0) {
        $exit_value = $exit_code;
    } elsif ($exit_code && 0xff) {
        $exit_value = "[SIGNAL $exit_code]";
    } else {
        $exit_value = $exit_code >> 8;
    }

    return {
        exit_code => $exit_code,
        exit_value => $exit_value,
        out => $out,
        err => $err,
    };
}

1;
