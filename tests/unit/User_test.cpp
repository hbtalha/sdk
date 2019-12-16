/**
 * (c) 2019 by Mega Limited, Wellsford, New Zealand
 *
 * This file is part of the MEGA SDK - Client Access Engine.
 *
 * Applications using the MEGA API must present a valid application key
 * and comply with the the rules set forth in the Terms of Service.
 *
 * The MEGA SDK is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * @copyright Simplified (2-clause) BSD License.
 *
 * You should have received a copy of the license along with this
 * program.
 */

#include <gtest/gtest.h>

#include <mega/megaclient.h>
#include <mega/megaapp.h>
#include <mega/user.h>

#include "DefaultedFileSystemAccess.h"
#include "utils.h"

namespace
{

class MockFileSystemAccess : public mt::DefaultedFileSystemAccess
{
public:
};

void checkUsers(mega::User& exp, mega::User& act)
{
    ASSERT_EQ(exp.userhandle, act.userhandle);
    ASSERT_EQ(exp.email, act.email);
    ASSERT_EQ(exp.show, act.show);
    ASSERT_EQ(exp.ctime, act.ctime);
    std::string expKey;
    exp.pubk.serializekey(&expKey, mega::AsymmCipher::PUBKEY);
    ASSERT_FALSE(expKey.empty());
    std::string actKey;
    act.pubk.serializekey(&actKey, mega::AsymmCipher::PUBKEY);
    ASSERT_FALSE(actKey.empty());
    ASSERT_EQ(expKey, actKey);
    ASSERT_EQ(*exp.getattr(mega::ATTR_FIRSTNAME), *act.getattr(mega::ATTR_FIRSTNAME));
    ASSERT_EQ(*exp.getattrversion(mega::ATTR_FIRSTNAME), *act.getattrversion(mega::ATTR_FIRSTNAME));
    ASSERT_EQ(*exp.getattr(mega::ATTR_LASTNAME), *act.getattr(mega::ATTR_LASTNAME));
}

}

TEST(User, serialize_unserialize)
{
    mega::MegaApp app;
    MockFileSystemAccess fsaccess;
    auto client = mt::makeClient(app, fsaccess);
    const std::string email = "foo@bar.com";
    mega::User user{email.c_str()};
    user.userhandle = 13;
    user.ctime = 14;
    user.show = mega::VISIBLE;
    std::string firstname1 = "f";
    std::string firstname2 = "f2";
    std::string lastname = "oo";
    user.setattr(mega::ATTR_FIRSTNAME, &firstname1, &firstname2);
    user.setattr(mega::ATTR_LASTNAME, &lastname, nullptr);
    std::string key(1024, 'X');
    user.pubk.setkey(mega::AsymmCipher::PUBKEY, reinterpret_cast<const mega::byte*>(key.c_str()), static_cast<int>(key.size()));

    std::string d;
    ASSERT_TRUE(user.serialize(&d));

    auto newUser = mega::User::unserialize(client.get(), &d);
    checkUsers(user, *newUser);
}
