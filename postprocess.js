fs=require('fs');
src=fs.readFileSync(process.argv[2],'utf8');
src=src.replace(/_ct_\w+_\d+:[\w\$]+,?/g, '');
src=src.replace(/\w+\._ct_\w+_\d+\=\w+._ct_\w+_\d+;/g, '');
src=src.replace(/;\s{2,}/g,'\n');
fs.writeFileSync(process.argv[3], src, 'utf8');
