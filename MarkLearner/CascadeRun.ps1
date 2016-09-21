if(![System.IO.Directory]::Exists([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "Cascades"))){
    [System.IO.Directory]::CreateDirectory([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "Cascades"))
}
$vecfiles = [System.IO.Directory]::GetFiles([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "vecs"), "*", [System.IO.SearchOption]::AllDirectories)
$negDic = [System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "negs")
$casDic = [System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "Cascades")
foreach($f in $vecfiles) {
    $ng_name = [System.IO.Path]::Combine($negDic, [System.IO.Path]::GetFileNameWithoutExtension($f) + ".txt")
    $cas_name = [System.IO.Path]::Combine($casDic, [System.IO.Path]::GetFileNameWithoutExtension($f))
    if(![System.IO.File]::Exists($ng_name)){
        continue
    }
    if([System.IO.Directory]::Exists($cas_name)){
        foreach($n in [System.IO.Directory]::GetFiles($cas_name)) {
            [System.IO.File]::Delete($n)
        }
    } else {
        [System.IO.Directory]::CreateDirectory($cas_name)
    }
    Start-Process -FilePath opencv_traincascade.exe -ArgumentList "-data `"$cas_name`" -vec `"$f`" -bg `"$ng_name`" -numPos 900 -numNeg 49"
    Write-Output "Learned `"$f`""
    break
}