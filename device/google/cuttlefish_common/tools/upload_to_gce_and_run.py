#!/usr/bin/python

"""Upload a local build to Google Compute Engine and run it."""

import argparse
import glob
import os
import subprocess


def upload_artifacts(args):
  dir = os.getcwd()
  try:
    os.chdir(args.image_dir)
    images = glob.glob('*.img')
    if len(images) == 0:
      raise OSError('File not found: %s' + image_pat)
    subprocess.check_call(
      'tar -c -f - --lzop -S ' + ' '.join(images) +
        ' | gcloud compute ssh %s@%s -- tar -x -f - --lzop -S' % (
          args.user,
          args.instance),
      shell=True)
  finally:
    os.chdir(dir)

  host_package = os.path.join(args.host_dir, 'cvd-host_package.tar.gz')
  # host_package
  subprocess.check_call(
      'gcloud compute ssh %s@%s -- tar -x -z -f - < %s' % (
          args.user,
          args.instance,
          host_package),
      shell=True)


def launch_cvd(args):
  if args.data_image:
    subprocess.check_call(
        'gcloud compute ssh %s@%s -- bin/launch_cvd --data-image %s '
        '--data-policy create_if_missing --blank-data-image-mb %d' % (
            args.user,
            args.instance,
            args.data_image,
            args.blank_data_image_mb),
        shell=True)
  else:
    subprocess.check_call(
        'gcloud compute ssh %s@%s -- bin/launch_cvd' % (
            args.user,
            args.instance),
        shell=True)


def stop_cvd(args):
  subprocess.call(
      'gcloud compute ssh %s@%s -- bin/stop_cvd' % (
          args.user,
          args.instance),
      shell=True)


def main():
  parser = argparse.ArgumentParser(
      description='Upload a local build to Google Compute Engine and run it')
  parser.add_argument(
      '-host_dir',
      type=str,
      default=os.environ.get('ANDROID_HOST_OUT', '.'),
      help='path to the dist directory')
  parser.add_argument(
      '-image_dir',
      type=str,
      default=os.environ.get('ANDROID_PRODUCT_OUT', '.'),
      help='path to the img files')
  parser.add_argument(
      '-instance', type=str, required=True,
      help='instance to update')
  parser.add_argument(
      '-user', type=str, default='vsoc-01',
      help='user to update on the instance')
  parser.add_argument(
      '-data-image', type=str, default=None,
      help='userdata image file name, this file will be used instead of default one')
  parser.add_argument(
      '-blank-data-image-mb', type=int, default=4098,
      help='custom userdata image size in megabytes')
  parser.add_argument(
      '-launch', default=False,
      action='store_true',
      help='launch the device')
  args = parser.parse_args()
  stop_cvd(args)
  upload_artifacts(args)
  if args.launch:
    launch_cvd(args)


if __name__ == '__main__':
  main()
